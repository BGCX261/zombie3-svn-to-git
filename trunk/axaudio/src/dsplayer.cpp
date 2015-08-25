#include "axaudioexception.hxx"
#include "config.hxx"
#include "priv/speexconverter.hxx"
#include "priv/dsplayer.hxx"

DSPlayer::DSPlayer(HWND hWnd,SpeexConverter* converter,FileLog* filelog)
:_hWnd(hWnd),
 _soundmanager(NULL),
 _streamsound(NULL),
 _eventhandle(NULL),
 _converter(converter),
 _is_shutdown_requested(FALSE),
 _filelog(filelog)
{
	if (!Init())
		throw AXAudioException("Failed to init DSPlayer!");
}
DSPlayer::~DSPlayer()
{
	Uninit();
}
void DSPlayer::Shutdown()
{
	_is_shutdown_requested = TRUE;
}
tBool DSPlayer::ReadyForPlayMore(void) const
{
	if (_streamsound==NULL)
		return FALSE;

	return _streamsound->ReadyForPlayMore();
}
tUInt32 DSPlayer::WriteSound(tByte* data,tUInt32 size)
{
	if (_streamsound==NULL)
		return 0;

	return _streamsound->WriteSound(data,size);
}
void DSPlayer::Execute(Thread::Arg)
{
	HRESULT hr;
    MSG     msg;
    DWORD   dwResult;
	DWORD   dwIndex;
    BOOL    bDone = FALSE;
    BOOL    bLooped;
	const BYTE* pPlayFrame;
	

	if (_streamsound==NULL)
		return ;

	if( FAILED( hr = _streamsound->Play( 0, DSBPLAY_LOOPING ) ) )
        return ;

    while( !_is_shutdown_requested ) 
    { 
		dwResult = MsgWaitForMultipleObjects( 1, &_eventhandle,FALSE, INFINITE, QS_ALLEVENTS );

		switch (dwResult)
		{
		case (WAIT_OBJECT_0 + 0):
#ifdef SPEEX_AEC
			if (NULL!=(pPlayFrame=GetLastPlayedFrame()))
				_converter->SpeexEchoPlayback(pPlayFrame);
#endif
			_streamsound->HandleWaveStreamNotification();
			break;
		case (WAIT_OBJECT_0 + 1):
			while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
            { 
                if( msg.message == WM_QUIT )
                    bDone = TRUE;
            }
			break;
		default:
			break;
		}
	}

	_streamsound->Stop();
}
tBool DSPlayer::Init()
{
	tUInt32 flags = 0/*DSBCAPS_CTRLVOLUME*/;

	_soundmanager = new	CSoundManager();

	HRESULT hr = _soundmanager->Initialize(_hWnd,DSSCL_PRIORITY,CHANNEL_NB,SAMPLE_RATE,BITS_PER_SAMPLE);
	if (FAILED(hr))
		goto FAIL_CLEANUP;

	_eventhandle = CreateEvent( NULL, FALSE, FALSE, NULL );

	if (_eventhandle==NULL)
		goto FAIL_CLEANUP;

	hr = _soundmanager->CreateStreaming(&_streamsound,flags,GUID_NULL,PLAYER_BLOCK_NB,BLOCK_SIZE,_eventhandle,_filelog);
	if (FAILED(hr))
		goto FAIL_CLEANUP;

	_streamsound->FillBufferWithSound(TRUE);	// fill it with silence.

	return TRUE;

FAIL_CLEANUP:
	Uninit();
	return FALSE;
}
tBool DSPlayer::Uninit()
{
	if (_streamsound)
	{
		_streamsound->Stop();

		delete _streamsound;
		_streamsound = NULL;
	}

	if (_eventhandle)
	{
		CloseHandle(_eventhandle);
		_eventhandle = NULL;
	}

	if (_soundmanager)
	{
		delete _soundmanager;
		_soundmanager = NULL;
	}

	return TRUE;
}
const tByte* DSPlayer::GetLastPlayedFrame(void) const
{
	return _streamsound->GetLastPlayedFrame();
}
const tByte* DSPlayer::GetNextPlayFrame(void) const
{
	return _streamsound->GetNextPlayFrame();
}
tUInt32 GetAUniqueID(void)
{
	const static tUInt32 TOP = 10000;
	const static tUInt32 BOTTOM = 0;
	static tUInt32 id_counter=BOTTOM;

	if (++id_counter>TOP)
		id_counter = BOTTOM;

	return id_counter;	
}