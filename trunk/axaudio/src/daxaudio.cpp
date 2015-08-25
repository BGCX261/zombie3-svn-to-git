#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "axaudioexception.hxx"
#include "config.hxx"
#include "priv/dsplayer.hxx"
#include "priv/dsrecorder.hxx"
#include "priv/speexconverter.hxx"
#include "priv/acmconverter.hxx"
#include "priv/speex_jitter_buffer.h"
#include "priv/daxaudio.hxx"

DAXAudio::DAXAudio(void* hWnd,FileLog* filelog)
:_hWnd((HWND)hWnd),
 _player(NULL),
 _recorder(NULL),
 _converter(NULL),
 _from_jitter_map(),
 _filelog(filelog)
{
	if (!Init())
		throw AXAudioException("Failed to init DAXAudio.");
}

DAXAudio::~DAXAudio()
{
	Uninit();
}
tBool DAXAudio::Start()
{
	return StartRecord() && StartPlay();
}
void DAXAudio::Stop()
{
	StopRecord();
	StopPlay();
}
tBool DAXAudio::StartRecord()
{
	if (!_recorder)
		return FALSE;

	if (_recorder->State()==Thread::RUNNING_STATE)
		return TRUE;

	_recorder->Start();

	return TRUE;
}
tBool DAXAudio::StartPlay()
{
	if (!_player)
		return FALSE;

	if (_player->State()==Thread::RUNNING_STATE)
		return TRUE;

	_player->Start();

	return TRUE;
}
void DAXAudio::StopRecord()
{
	if (_recorder)
		_recorder->Shutdown();
}
void DAXAudio::StopPlay()
{
	if (_player)
		_player->Shutdown();
}
void DAXAudio::RegisterFrom(string host,tUInt32 port)
{
#ifdef USING_JITTER
	// check if already exists
	tFromJitterMapIter iter = _from_jitter_map.find(from);

	if (iter!=_from_jitter_map.end())
		return ;

	SpeexJitter* jitter = new SpeexJitter();

	speex_jitter_init(jitter,_converter->GetDecoder(),SAMPLE_RATE);
	_from_jitter_map.insert(tFromJitterPair(from,jitter));
#endif
}
void DAXAudio::UnregisterFrom(string host,tUInt32 port)
{
#ifdef USING_JITTER
	// check if already exists
	tFromJitterMapIter iter = _from_jitter_map.find(from);

	if (iter!=_from_jitter_map.end())
		return ;

	speex_jitter_destroy(iter->second);
	delete iter->second;
	_from_jitter_map.erase(iter);
#endif
}
tUInt32 DAXAudio::GetBlockSize(void) const
{
	return BLOCK_SIZE;
}
tUInt32 DAXAudio::WriteSound(string host,tUInt32 port,tByte* data,tUInt32 size)
{
	if (!_player)
		return 0;

#ifdef SPEEX_CODE
	tByte frame[BLOCK_SIZE]={0};
	int voicesize = *((tUInt32*)(data));
	int timestamp = *((tUInt32*)(data+4));

	if (timestamp % FRAME_SIZE != 0)
	{
		LOGWARNING("DAXAudio::WriteSound:Currupted data with timestamp "<<timestamp<<" from "<<host<<"::"<<port<<",abandon the data!");
		return 0;
	}

	if (voicesize!=size-8)
	{
		LOGWARNING("DAXAudio::WriteSound:Currupted data from "<<host<<" for "<<voicesize<<"!="<<size<<"-8,abandon the data!");
		return 0;
	}

#ifdef USING_JITTER
	// find the right jitter and stored the data into
	// check if player can playmore,if yes, get a frame from all jitters and merge them
	// write the merged data into player.
	tByte from[100]={0};
	sprintf_s((char*)from,100,"%s::%u",host.c_str(),port);
	tFromJitterMapIter iter = _from_jitter_map.find(from);

	if (iter==_from_jitter_map.end())
	{
		LOGWARNING("DAXAudio::WriteSound:From "<<from<<" is not registered,abandon the data!");
		return 0;
	}

	speex_jitter_put(iter->second,(char*)(data+8),size-8,timestamp);

	if (_player->ReadyForPlayMore())
	{
		GetMergedFramFromJitters(frame,BLOCK_SIZE);
		_player->WriteSound(frame,BLOCK_SIZE);
		return size;
	}
	else
	{
		LOGWARNING("DAXAudio::WriteSound:Player is busy,wait a momment...");
		return 0;
	}
#else	
	if (_converter->Decode(data+8,size-8,frame,BLOCK_SIZE))
		if(_player->WriteSound(frame,BLOCK_SIZE))
			return size;
	return 0;
#endif	//USING_JITTER

#else
	// just for test, of course coded and sent.
	//if (_player->ReadyForPlayMore())
		return _player->WriteSound(data,size);
	//else
	//	return 0;
#endif	//SPEEX_CODE
}
tUInt32 DAXAudio::ReadSound(tByte* buf, tUInt32 bufsize)
{
#ifdef SPEEX_CODE

	tUInt32 codedsize = 0;
	static LONG lTimeStamp=0;

	tByte aCodedFrame[BLOCK_SIZE]={0};
	tUInt32 block_size = _recorder->ReadSound(aCodedFrame,BLOCK_SIZE);

	if (block_size==0)
	{
		//LOGDEBUG("DAXAudio::ReadSound:Read No data from recorder,it's empty.");
		return 0;
	}

	codedsize = _converter->Code(aCodedFrame,block_size,buf+8,bufsize-8);

	if (codedsize==0)
	{
		LOGDEBUG("DAXAudio::ReadSound:Get encoded no data for DTX.");
		return 0;
	}

	*((tUInt32*)(buf+0)) = codedsize;
	*((tUInt32*)(buf+4)) = lTimeStamp;

	lTimeStamp += FRAME_SIZE;

	return codedsize+8;

#else
	return _recorder->ReadSound(buf,bufsize);
#endif
}

void DAXAudio::SetOutVolume(tUInt32 val)
{}
tUInt32 DAXAudio::GetOutVolume(void)
{
	return 0;
}

void DAXAudio::SetInVolume(tUInt32 val)
{
}
tUInt32 DAXAudio::GetInVolume(void)
{
	return 0;
}
 DAXAudio::DAXAudio(const DAXAudio& model)
 {}
tBool DAXAudio::Init()
{
	_converter = new SpeexConverter();
	//_converter = new ACMConverter(ENCODE_FORMAT,_filelog);

	_player = new DSPlayer(_hWnd,_converter/*NULL*/,_filelog);

	//_player->Start();

	_recorder = new DSRecorder(_hWnd,_converter/*NULL*/,_filelog);

	//_recorder->Start();

	return TRUE;
}
tBool DAXAudio::Uninit()
{
	if (_player) _player->Shutdown();

	tUInt32 uCounter=1000;
	while (uCounter-- && (_player || _recorder))
	{
		if (_player && _player->State()==Thread::FINISHED_STATE) 
		{
			_player->Join();
			delete _player;
			_player = NULL;
		}

		if (_recorder && _recorder->State()==Thread::FINISHED_STATE) 
		{
			_recorder->Join();
			delete _recorder;
			_recorder = NULL;
		}

		Thread::Yield();
	}

	if (uCounter==0)
	{
		LOGWARNING("DAXAudio::Uninit:Some threads finished,but sorry we won't join, force shutting down...");
		if (_player) {delete _player; _player=NULL;}
		if (_recorder) {delete _recorder; _recorder=NULL;}
	}

	delete _converter;
	_converter = NULL;

	return TRUE;
}
void DAXAudio::GetMergedFramFromJitters(tByte* buf,tUInt32 bufsize)
{
	short frame[FRAME_SIZE]={0};

	_ASSERT(bufsize>=FRAME_SIZE*2);

	memset(buf,0x00,bufsize);

	tFromJitterMapIter iter = _from_jitter_map.begin();

	while (iter != _from_jitter_map.end())
	{
		LOGDEBUG("DAXAudio::GetMergedFramFromJitters:Get a fram from the Jitter.");

		speex_jitter_get(iter->second,frame,NULL);

		//MixVoice((short*)buf,frame,FRAME_SIZE);
		memcpy(buf,frame,BLOCK_SIZE);
		break;

		iter++;
	}
}
void DAXAudio::MixVoice(tInt16* voice1,const tInt16* voice2,tUInt32 len)
{
	for (tUInt32 i=0;i<len;i++)
	{
		*(voice1+i) = *(voice1+i) + *(voice2+i);
	}
}