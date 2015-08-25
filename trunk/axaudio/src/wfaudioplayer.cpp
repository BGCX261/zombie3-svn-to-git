
/**
 * to invalidate the winsock.h in windows.h file
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <mmsystem.h>
#include "config.hxx"

#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "axaudioexception.hxx"
#include "priv/speexconverter.hxx"
#include <speex/speex_jitter.h>
#include "priv/speex_jitter_buffer.h"	// speex_jitter_init,SpeexJitter
#include "priv/wfaudioplayer.hxx"

static const tUInt32 FULL_VOLUME = 0xFFFF;
static const tUInt32 SILENCE_VOLUME = 0x0000;

WFAudioPlayer::WFAudioPlayer(SpeexConverter* converter,FileLog* filelog)
		:	_converter(converter),
			_current_block(0),
			_free_block_count(PLAYER_BLOCK_NB),
			_critical_section_on_free_block_count(),
			_hwaveout(NULL),
			_blocks(NULL),
			_host_buffer_map(),
			_filelog(filelog)
{
	if(!Init())
		throw AXAudioException("Failed to init WFAudioPlayer!");
}

WFAudioPlayer::~WFAudioPlayer()
{
	Uninit();

	for (tHostBufferMapIter iter=_host_buffer_map.begin();iter!=_host_buffer_map.end();iter++)
	{
		speex_jitter_destroy(iter->second);
	}
}
void WFAudioPlayer::WriteSound(string fromendpoint,tByte* data,tUInt32 size)
{
	int recv_timestamp = ((int*)data)[1];

	if (recv_timestamp%(FRAME_SIZE) !=0)
	{
		LOGWARNING("WFAudioPlayer::WriteSound:Illegal data from "<<fromendpoint<<",for timestamp is "<<recv_timestamp<<",abandon it.");
		return;
	}

	SpeexJitter* jitter;
	tHostBufferMapIter iter = _host_buffer_map.find(fromendpoint);

	// get the jitter buffer.
	if (iter==_host_buffer_map.end())
	{
		LOGDEBUG("WFAudioPlayer::WriteSound:No SpeexJitter for "<<fromendpoint<<",new it.");

		jitter = new SpeexJitter();

		speex_jitter_init(jitter,_converter->GetDecoder(),SAMPLE_RATE);
		_host_buffer_map.insert(tHostBufferMapPair(fromendpoint,jitter));
	}
	else
		jitter = iter->second;

	speex_jitter_put(jitter, reinterpret_cast<char*>(data+8),size-8, recv_timestamp);

	MoveJitteredVoiceToPlayer();
}
void WFAudioPlayer::SetOutVolume(tUInt32 val)
{
	tUInt32 real_val = ((float)val/10)*(FULL_VOLUME);

	if (MMSYSERR_NOERROR!=waveOutSetVolume(_hwaveout,real_val))
		LOGWARNING("WFAudioPlayer::SetOutVolume:Failed to set waveout volume to "<<real_val<<".");
}
tUInt32 WFAudioPlayer::GetOutVolume(void)
{
	tUInt32 val;

	if (MMSYSERR_NOERROR!=waveOutGetVolume(_hwaveout,(LPDWORD)&val))
		return 0;
	else
		return (tUInt32)(((float)val/(float)FULL_VOLUME)*10);
}
void __stdcall WFAudioPlayer::WaveOutProc(HWAVEOUT hWaveOut,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	if(uMsg != WOM_DONE)
		return;

	WFAudioPlayer* player = reinterpret_cast<WFAudioPlayer*>(dwInstance);

	if (player)
	{
		player->IncrFreeBlockCount();

		WAVEHDR* pwh = (WAVEHDR*)dwParam1;

		//tByte* nextframe = player->GetNextBlockFrame();

		//if (nextframe)
		//	player->GetConverter()->SpeexEchoPlayback(nextframe);

		player->GetConverter()->SpeexEchoPlayback(reinterpret_cast<tByte *>(pwh->lpData));
	}
}
tBool WFAudioPlayer::Init()
{
	AllocateBlocks(PLAYER_BLOCK_NB,FRAME_SIZE*TWO_BYTES);

	MMRESULT mmr;
	WAVEFORMATEX wfx; 

	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = CHANNEL_NB;;
	wfx.wBitsPerSample = BITS_PER_SAMPLE;
	wfx.nSamplesPerSec = SAMPLE_RATE;
	wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample >> 3);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	 if(mmr = waveOutOpen(&_hwaveout,
					WAVE_MAPPER,
					&wfx,
					(DWORD_PTR)WaveOutProc,
					(DWORD_PTR)this,
					WAVE_FORMAT_DIRECT|CALLBACK_FUNCTION)	!=	MMSYSERR_NOERROR)
	 {
		 LOGERROR("WFAudioPlayer::Init:Failed to open wave out with returned value:"<<mmr);

		 return FALSE;
	 }

	return TRUE;
}

tBool WFAudioPlayer::Uninit()
{
	FreeBlocks();

	waveOutReset(_hwaveout);

	waveOutClose(_hwaveout);

	return TRUE;
}
tByte* WFAudioPlayer::AllocateBlocks(tUInt32 count, tUInt32 size)
{
	tByte* buffer;

	tUInt32 totalBufferSize = (size + sizeof(WAVEHDR)) * count;

	LOGDEBUG("WFAudioPlayer::AllocateBlocks started");


    /**
     * allocate memory for the entire set in one go
     */
	if (NULL == (buffer = (tByte*)malloc(totalBufferSize*sizeof(tByte))))
	{
		LOGERROR("WFAudioRecorder::AllocateBlocks:HeapAlloc failed.");
        return NULL;
	}
	memset((void*)buffer, 0x00, totalBufferSize);

    /**
     * and set up the pointers to each bit
     */
    _blocks = (WAVEHDR*)buffer;
    buffer += sizeof(WAVEHDR) * count;

    for(tUInt32 i = 0; i < count; i++) 
	{
		_blocks[i].dwBufferLength = size;
		_blocks[i].lpData = (LPSTR)buffer;
		buffer += size;

		//LOGDEBUG("WFAudioPlayer::AllocateBlocks:Block #"<<i<<" address starts at "<<reinterpret_cast<tUInt32>(_blocks[i].lpData));
	}

	LOGDEBUG("WFAudioPlayer::AllocateBlocks:Ended");

    return (tByte*)_blocks;
}
void WFAudioPlayer::FreeBlocks(void)
{
    if (_blocks)
	{
		free(_blocks);
		_blocks = NULL;
	}
}


tInt32 WFAudioPlayer::IncrFreeBlockCount(void)
{
	tUInt32 ret;

	_critical_section_on_free_block_count.EnterCriticalSection();

	if (_free_block_count < PLAYER_BLOCK_NB)
		_free_block_count++;

	ret = _free_block_count;

	_critical_section_on_free_block_count.LeaveCriticalSection();

	return ret;
}
tInt32 WFAudioPlayer::DecrFreeBlockCount(void)
{
	tUInt32 ret;

	_critical_section_on_free_block_count.EnterCriticalSection();
	
	if (_free_block_count>0)
		ret = --_free_block_count;
	else
		ret = -1;

	_critical_section_on_free_block_count.LeaveCriticalSection();

	return ret;
}
tByte* WFAudioPlayer::GetNextBlockFrame(void)
{
	tByte* nextblockframe = NULL;

	_critical_section_on_free_block_count.EnterCriticalSection();

	if (_free_block_count<PLAYER_BLOCK_NB);
	{
		WAVEHDR* current = &_blocks[(_current_block+_free_block_count)%PLAYER_BLOCK_NB];

		nextblockframe = reinterpret_cast<tByte*>(current->lpData);
	}

	_critical_section_on_free_block_count.LeaveCriticalSection();

	return nextblockframe;
}
void WFAudioPlayer::MoveJitteredVoiceToPlayer(void)
{
	//////////////////////////////////////////////////////
	if (DecrFreeBlockCount() < 0)
	{
		LOGDEBUG("WFAudioPlayer::WriteSound:The player is not ready for playback new data yet.");
		return;
	}

	const tByte* mixed_voice_frame;

	if (!(mixed_voice_frame=GetMixedInVoiceFrame()))
	{
		LOGWARNING("WFAudioPlayer::WriteSound:No data in jitter(s),courious!!!");
		IncrFreeBlockCount();
		return;
	}

	WAVEHDR* current = &_blocks[_current_block];
	
	/* 
	* first make sure the header we're going to use is unprepared
	*/
	if(current->dwFlags & WHDR_PREPARED)
	{
		if(MMSYSERR_NOERROR == waveOutUnprepareHeader(_hwaveout, current, sizeof(WAVEHDR)))
			;//LOGDEBUG("WFAudioPlayer::WriteSound:waveOutUnprepareHeader block #"<<_current_block<<" succeeded ...");
		else
			LOGWARNING("WFAudioPlayer::WriteSound:waveOutUnprepareHeader failed at  block #"<<_current_block<<",_free_block_count="<<_free_block_count);
	}

	memcpy(current->lpData,mixed_voice_frame,FRAME_SIZE*TWO_BYTES);
	current->dwBufferLength = FRAME_SIZE*TWO_BYTES;

	if (MMSYSERR_NOERROR == waveOutPrepareHeader(_hwaveout, current, sizeof(WAVEHDR)) && 
		MMSYSERR_NOERROR == waveOutWrite(_hwaveout, current, sizeof(WAVEHDR)))
		LOGDEBUG("WFAudioPlayer::WriteSound:waveOutWrite to block #"<<_current_block<<" succeeded");
	else
		LOGWARNING("WFAudioPlayer::WriteSound:waveOutWrite failed at block#"<<_current_block<<" _free_block_count:"<<_free_block_count);

	_current_block++;
	_current_block %= PLAYER_BLOCK_NB;
}
const tByte* WFAudioPlayer::GetMixedInVoiceFrame(void)
{
	short tmpframe[FRAME_SIZE]={0};

	memset(_mixed_invoice_data,0x00,FRAME_SIZE*TWO_BYTES);

	for (tHostBufferMapIter iter=_host_buffer_map.begin();iter!=_host_buffer_map.end();++iter)
	{
		speex_jitter_get(iter->second, tmpframe, NULL);

		//LOGDEBUG("WFAudioPlayer::GetMixedInVoiceFrame");

		MixVoice(reinterpret_cast<tInt16*>(_mixed_invoice_data),tmpframe,FRAME_SIZE);
		//memcpy(_mixed_invoice_data,_tmpframe,FRAME_SIZE*TWO_BYTES);
	}

	return (tByte*)_mixed_invoice_data;
}
void WFAudioPlayer::MixVoice(tInt16* voice1,const tInt16* voice2,tUInt32 len)
{
	for (tUInt32 i=0;i<len;i++)
	{
		*(voice1+i) = *(voice1+i) + *(voice2+i);
	}
}

SpeexConverter* WFAudioPlayer::GetConverter() const
{
	return _converter;
}
tInt32 WFAudioPlayer::GetCurrentPos(void)
{
	MMRESULT mmr;
	MMTIME  mmt;
	tInt32 currentpos = -1;

	mmt.wType = TIME_BYTES;
	
	mmr = waveOutGetPosition(_hwaveout,&mmt,sizeof(MMTIME));

	if (mmr == MMSYSERR_NOERROR && mmt.u.cb > 0)
	{
		{
			currentpos = mmt.u.cb % (PLAYER_BLOCK_NB*FRAME_SIZE*TWO_BYTES);
		}
	}

	return currentpos;
}