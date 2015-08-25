#include "myfilelog.hxx"
#include "loghelper.hxx"

#include "priv/speexconverter.hxx"
#include "priv/wfaudioplayer.hxx"
#include "priv/wfaudiorecorder.hxx"

#include "axaudioexception.hxx"
#include "priv/waveaxaudio.hxx"

#pragma comment(lib,"mythread")
#pragma comment(lib,"myfile")

WaveAXAudio::WaveAXAudio(FileLog* filelog)
:_converter(NULL),
 _player(NULL),
 _recorder(NULL),
 _filelog(filelog)
{
	if (!Init())
		throw AXAudioException("Failed to init WaveAXAudio!");
}
WaveAXAudio::~WaveAXAudio()
{
	Uninit();
}
void WaveAXAudio::WriteSound(string fromendpoint,tByte* audio,tUInt32 size)
{
	if (_player)
		_player->WriteSound(fromendpoint,audio,size);
}
tUInt32 WaveAXAudio::ReadSound(tByte* buf,tUInt32 size)
{
	if(!_recorder)
		return 0;

	return _recorder->ReadRecord(buf,size);
}
void WaveAXAudio::SetOutVolume(tUInt32 val)
{
	if (_player)
		_player->SetOutVolume(val);
}
tUInt32 WaveAXAudio::GetOutVolume(void)
{
	if (_player)
		return _player->GetOutVolume();
	else 
		return 0;
}
void WaveAXAudio::SetInVolume(tUInt32 val)
{
	if (_recorder)
		_recorder->SetInVolume(val);
}
tUInt32 WaveAXAudio::GetInVolume(void)
{
	if (_recorder)
		return _recorder->GetInVolume();
	else 
		return 0;
}
tBool WaveAXAudio::Init()
{
	try
	{
		_converter = new SpeexConverter();
	}
	catch (AXAudioException& e)
	{
		if (_converter) delete _converter;
		return FALSE;
	}


	try
	{
		_player = new WFAudioPlayer(_converter,_filelog);
	}
	catch (AXAudioException& e)
	{
		if (_player)
		{
			delete _player;
			_player = NULL;
		}
	}

	try
	{
		_recorder = new WFAudioRecorder(_converter,_filelog);
	}
	catch (AXAudioException& e)
	{
		if (_recorder)
		{
			delete _recorder;
			_recorder = NULL;
		}
	}

	return TRUE;
}
tBool WaveAXAudio::Uninit()
{
	if (_player) delete _player;
	if (_recorder) delete _recorder;
	if (_converter) delete _converter;

	return TRUE;
}