#include <iostream>
#include <fstream>
#include "config.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"

#include "priv/speexconverter.hxx"
#include "axaudioexception.hxx"
#include "priv/wfaudiorecorder.hxx"


using namespace std;

WFAudioRecorder::WFAudioRecorder(SpeexConverter* converter,FileLog* filelog)
:	_converter(converter),
	_current_block(0),
	_free_block_count(RECORDER_BLOCK_NB),
	_recorded_data_blocks(NULL),
	_cur_write_recorded_data_block(0),
	_cur_read_recorded_data_block(0),
	_recorded_data_criticalsection(),
	_blocks(NULL),
	_device(NULL),
	_shutdown_requested(FALSE),
	_filelog(filelog)
{
	if (!Init())
		throw AXAudioException("Failed to init WFAudioRecorder!");

	_recorded_data_blocks = new tCodedData[RECORDED_BLOCK_NB];

	for (tUInt32 i=0;i<RECORDED_BLOCK_NB;i++)
	{
		_recorded_data_blocks[i].codeddata = new tByte[FRAME_SIZE*TWO_BYTES];
		_recorded_data_blocks[i].codedsize = 0;
	}
}
WFAudioRecorder::~WFAudioRecorder()
{
	Uninit();

	for (tUInt32 i=0;i<RECORDED_BLOCK_NB;i++)
		delete _recorded_data_blocks[i].codeddata;

	delete[] _recorded_data_blocks;
}

void WFAudioRecorder::Shutdown()
{
	_shutdown_requested = TRUE;
}
tUInt32 WFAudioRecorder::ReadRecord(tByte* buf, tUInt32 size)
{
	tUInt32 ret = 0;

	_recorded_data_criticalsection.EnterCriticalSection();
	
	if (_cur_read_recorded_data_block!=_cur_write_recorded_data_block)
	{
		if (size>_recorded_data_blocks[_cur_read_recorded_data_block].codedsize)
		{
			ret = _recorded_data_blocks[_cur_read_recorded_data_block].codedsize;

			memcpy(buf,_recorded_data_blocks[_cur_read_recorded_data_block].codeddata,ret);
			_cur_read_recorded_data_block++;
			_cur_read_recorded_data_block %= RECORDED_BLOCK_NB;
		}
	}

	_recorded_data_criticalsection.LeaveCriticalSection();

	/**
	 * it was put in the waveinProc,but according to MSDN:
	 * Applications should not call any system-defined functions from inside a callback function, except for EnterCriticalSection, LeaveCriticalSection, OutputDebugString, PostMessage, PostThreadMessage, and SetEvent. Calling other wave functions will cause deadlock.
	 */
	// since it's time consuming,do it every 1000th.
	static tUInt32 timecounter=0;
	if(++timecounter>2)
	{
		timecounter = 0;
		PrepareInHeaders();
	}

	return ret;
}
void WFAudioRecorder::SetInVolume(tUInt32 val)
{}
tUInt32 WFAudioRecorder::GetInVolume(void)
{
	return 0;
}
void __stdcall WFAudioRecorder::WaveInProc(HWAVEIN hWaveIn,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	if(uMsg != WIM_DATA)
		return;

	static tUInt32 timecounter = 0;
	
	WFAudioRecorder* recorder = reinterpret_cast<WFAudioRecorder*>(dwInstance);

	if (recorder)
	{
		recorder->IncrFreeBlockCount();

		WAVEHDR* pwh = (WAVEHDR*)dwParam1;
		
		recorder->ProcessRecordedFrame(reinterpret_cast<tByte*>(pwh->lpData));
	}
}
tBool WFAudioRecorder::Init()
{
	if (!AllocateBlocks(RECORDER_BLOCK_NB,FRAME_SIZE*TWO_BYTES))
	{
		LOGERROR("WFAudioRecorder::Init:AllocateBlocks("<<RECORDER_BLOCK_NB<<","<<FRAME_SIZE*TWO_BYTES<<") failed.");
		return FALSE;
	}

	/* Get the first available device index */
	tUInt32 count = waveInGetNumDevs();
	tUInt32 device_index = 0;
	WAVEINCAPS caps;
	for (tUInt32 i = 0; i < count; i++)
	{
		if (waveInGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR)
		{
			device_index++;

			break;
		}
	}
	if (device_index == 0)
	{
		LOGERROR("WFAudioRecorder::Init:No device available.");

		return FALSE;
	}

	/* Open the device */
	WAVEFORMATEX format;;
	format.cbSize = 0;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = CHANNEL_NB;;
	format.wBitsPerSample = BITS_PER_SAMPLE;
	format.nSamplesPerSec = SAMPLE_RATE;
	format.nBlockAlign = format.nChannels * (format.wBitsPerSample >> 3);
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

	if (waveInOpen(&_device,
				WAVE_MAPPER,
				&format,
				(DWORD_PTR)WaveInProc,
				(DWORD_PTR)this,
				WAVE_FORMAT_DIRECT|CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	{
		LOGERROR("WFAudioRecorder::Init:waveInOpen failed.");

		return FALSE;
	}

	if (waveInStart(_device) != MMSYSERR_NOERROR)
	{
		LOGERROR("WFAudioRecorder::Init:waveInStart failed.");

		return FALSE;
	}

	PrepareInHeaders();

	return TRUE;
}

tBool WFAudioRecorder::Uninit()
{
	tBool ret = TRUE;

	MMRESULT mmr;

	//mmr = waveInReset(_device);

	//if (mmr != MMSYSERR_NOERROR)
	//{
	//	LOGERROR("WFAudioRecorder::Uninit:waveInReset failed with return value = "<<mmr<<".");

	//	ret = FALSE;
	//}

	mmr = waveInStop(_device);

	if (mmr != MMSYSERR_NOERROR)
	{
		LOGERROR("WFAudioRecorder::Uninit:waveInStop failed with return value = "<<mmr<<".");

		ret = FALSE;
	}

	mmr = waveInReset(_device);

	//if (mmr != MMSYSERR_NOERROR)
	//{
	//	LOGERROR("WFAudioRecorder::Uninit:waveInReset failed with return value = "<<mmr<<".");

	//	ret = FALSE;
	//}

	mmr = waveInClose(_device);

	if (mmr != MMSYSERR_NOERROR)
	{
		LOGERROR("WFAudioRecorder::Uninit:waveInClose failed with return value = "<<mmr<<".");

		ret = FALSE;
	}

	FreeBlocks();

	return ret;
}
tInt32 WFAudioRecorder::IncrFreeBlockCount(void)
{
	if (_free_block_count < RECORDER_BLOCK_NB)
		_free_block_count++;

	return _free_block_count;
}
tInt32 WFAudioRecorder::DecrFreeBlockCount(void)
{	
	if (_free_block_count>0)
		return --_free_block_count;
	else
		return -1;
}
tByte* WFAudioRecorder::AllocateBlocks(tUInt32 count, tUInt32 size)
{
	tByte* buffer;

    tUInt32 totalBufferSize = (size + sizeof(WAVEHDR)) * count;

	//assert(count && size);

    /**
     * allocate memory for the entire set in one go
     */
	if (NULL == (buffer = (tByte*)malloc(totalBufferSize*sizeof(tByte))))
	{
		fprintf(stderr, "Memory allocation error\n");

		LOGERROR("WFAudioRecorder::AllocateBlocks:	HeapAlloc failed.");

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
	}

    return (tByte*)_blocks;
}
void WFAudioRecorder::FreeBlocks()
{
    /* 
    * and this is why allocateBlocks works the way it does
    */ 
    if (_blocks)
	{
		free(_blocks);
		_blocks = NULL;
	}
}

void WFAudioRecorder::ProcessRecordedFrame(tByte* record)
{
	static tUInt32 timestamp_counter = 0;

	short echooutframe[FRAME_SIZE];
	char codeoutframe[FRAME_SIZE*TWO_BYTES];

	_converter->SpeexEchoCapture(record,echooutframe);

	if (0==_converter->SpeexPreprocessRun(echooutframe))
	{
		LOGDEBUG("WFAudioRecorder::ProcessRecordedFrame:Noise/silence,just abandon it.");
		return ;
	}

	tUInt32 codedsize = _converter->Code((tByte*)echooutframe,FRAME_SIZE,(tByte*)(codeoutframe+8),FRAME_SIZE*TWO_BYTES-8);

	*((tUInt32*)codeoutframe) = 0;	// null header

	timestamp_counter+=FRAME_SIZE;
	*(((tUInt32*)codeoutframe)+1) = timestamp_counter;

	_recorded_data_criticalsection.EnterCriticalSection();
	
	memcpy(_recorded_data_blocks[_cur_write_recorded_data_block].codeddata,codeoutframe,8+codedsize);
	_recorded_data_blocks[_cur_write_recorded_data_block].codedsize = 8+codedsize;

	_cur_write_recorded_data_block++;
	_cur_write_recorded_data_block %= RECORDED_BLOCK_NB;

	if (_cur_write_recorded_data_block==_cur_read_recorded_data_block)
	{	
		_cur_read_recorded_data_block++;
		_cur_read_recorded_data_block %= RECORDED_BLOCK_NB;
	}

	_recorded_data_criticalsection.LeaveCriticalSection();
}
void WFAudioRecorder::PrepareInHeaders(void)
{
	WAVEHDR* current;

	while (DecrFreeBlockCount()>=0)
	{
		current = &_blocks[_current_block];

		// Unprepare current block
		if ((current->dwFlags & WHDR_DONE) == WHDR_DONE)
		{
			if(MMSYSERR_NOERROR == waveInUnprepareHeader(_device, current, sizeof(WAVEHDR)))
				;//LOGDEBUG("WFAudioRecorder::PrepareInHeaders:waveInUnprepareHeader ...");
			else
				LOGERROR("WFAudioRecorder::PrepareInHeaders:waveInUnprepareHeader failed at  _current_block:"<<_current_block<<" _free_block_count:"<<_free_block_count);
		}

		// Prepare current block and add it in for recording.
		if (waveInPrepareHeader(_device, current, sizeof(WAVEHDR)) == MMSYSERR_NOERROR &&
			MMSYSERR_NOERROR == waveInAddBuffer(_device, current, sizeof(WAVEHDR)))
			;//LOGDEBUG("WFAudioRecorder::PrepareInHeaders:waveInAddBuffer:_current_block:"<<_current_block<<" _free_block_count:"<<_free_block_count<<".");
		else
			LOGERROR("WFAudioRecorder::PrepareInHeaders:waveInAddBuffer:failed at _current_block:"<<_current_block<<" _free_block_count:"<<_free_block_count);

		// Move forward current block.
		_current_block++;
		_current_block %= RECORDER_BLOCK_NB;
	}
}