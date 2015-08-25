#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "axaudioexception.hxx"
#include "config.hxx"
//#include "priv/dsplayer.hxx"
//#include "priv/dsrecorder.hxx"
#include "priv/speexconverter.hxx"
#include "priv/acmconverter.hxx"
#include "priv/speex_jitter_buffer.h"
#include "priv/dduplexaudio.hxx"

#define MAX(a,b)        ( (a) > (b) ? (a) : (b) )

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


DDuplexAudio::DDuplexAudio(void* hWnd,FileLog* filelog)
:m_hWnd((HWND)hWnd),
 m_pDSFD(NULL),
 m_pDSCB(NULL),
 m_pDSB(NULL),
 m_dwNextCaptureOffset(0),
 m_dwCaptureBufferSize(0),
 m_dwNextWriteOffset(0),
 m_dwWriteBufferSize(0),
 _events(),
 _from_jitter_map(),
 _warning(),
 _is_shutdown_requested(FALSE),
 _filelog(filelog)
{
	if (!Init())
		throw AXAudioException("初始化语音模块失败,请检查声卡!");
}

DDuplexAudio::~DDuplexAudio()
{
	Uninit();
}
void DDuplexAudio::Shutdown()
{
	_is_shutdown_requested = TRUE;
}
//tBool DDuplexAudio::StartRecord()
//{
//	if (!_recorder)
//		return FALSE;
//
//	if (_recorder->State()==Thread::RUNNING_STATE)
//		return TRUE;
//
//	_recorder->Start();
//
//	return TRUE;
//}
//tBool DDuplexAudio::StartPlay()
//{
//	if (!_player)
//		return FALSE;
//
//	if (_player->State()==Thread::RUNNING_STATE)
//		return TRUE;
//
//	_player->Start();
//
//	return TRUE;
//}
//void DDuplexAudio::StopRecord()
//{
//	if (_recorder)
//		_recorder->Shutdown();
//}
//void DDuplexAudio::StopPlay()
//{
//	if (_player)
//		_player->Shutdown();
//}
void DDuplexAudio::RegisterFrom(string host,tUInt32 port)
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
void DDuplexAudio::UnregisterFrom(string host,tUInt32 port)
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
tUInt32 DDuplexAudio::GetBlockSize(void) const
{
	return BLOCK_SIZE;
}
tUInt32 DDuplexAudio::WriteSound(string host,tUInt32 port, tByte* data, tUInt32 size)
{
#ifdef SPEEX_CODE
	tByte frame[BLOCK_SIZE]={0};
	int voicesize = *((tUInt32*)(data));
	int timestamp = *((tUInt32*)(data+4));

	if (timestamp % FRAME_SIZE != 0)
	{
		LOGWARNING("DDuplexAudio::WriteSound:Currupted data with timestamp "<<timestamp<<" from "<<host<<",abandon the data!");
		return 0;
	}

	if (voicesize!=size-8)
	{
		LOGWARNING("DDuplexAudio::WriteSound:Currupted data from "<<host<<" for "<<voicesize<<"!="<<size<<"-8,abandon the data!");
		return 0;
	}

#ifdef USING_JITTER
	// find the right jitter and stored the data into
	// check if player can playmore,if yes, get a frame from all jitters and merge them
	// write the merged data into player.
	tFromJitterMapIter iter = _from_jitter_map.find(from);

	if (iter==_from_jitter_map.end())
	{
		LOGWARNING("DDuplexAudio::WriteSound:From "<<from<<" is not registered,abandon the data!");
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
		LOGWARNING("DDuplexAudio::WriteSound:Player is busy,wait a momment...");
		return 0;
	}
#else	
	if (_converter->Decode(data+8,size-8,frame,BLOCK_SIZE))
		if(m_pBufferPlayer->WriteData(frame,BLOCK_SIZE))
			return size;
		else 
			return 0;
	else
		return 0;
#endif	//USING_JITTER

#else
	// just for test, of course coded and sent.
	//if (_player->ReadyForPlayMore())
	return m_pBufferPlayer->WriteData(data,size);
	//else
	//	return 0;
#endif	//SPEEX_CODE
}
tUInt32 DDuplexAudio::ReadSound(tByte* buf, tUInt32 bufsize)
{
#ifdef SPEEX_CODE

	tUInt32 codedsize = 0;
	static LONG lTimeStamp=0;

	tByte aCodedFrame[BLOCK_SIZE]={0};
	tUInt32 block_size = m_pBufferRecorder->ReadBlock(aCodedFrame,BLOCK_SIZE);

	if (block_size==0)
	{
		//LOGDEBUG("DDuplexAudio::ReadSound:Read No data from recorder,it's empty.");
		return 0;
	}

	codedsize = _converter->Code(aCodedFrame,block_size,buf+8,bufsize-8);

	if (codedsize==0)
	{
		LOGDEBUG("DDuplexAudio::ReadSound:Get encoded no data for DTX.");
		return 0;
	}

	*((tUInt32*)(buf+0)) = codedsize;
	*((tUInt32*)(buf+4)) = lTimeStamp;

	lTimeStamp += FRAME_SIZE;

	return codedsize+8;

#else
	return m_pBufferRecorder->ReadBlock(buf,bufsize);
#endif
}

void DDuplexAudio::SetOutVolume(tUInt32 val)
{}
tUInt32 DDuplexAudio::GetOutVolume(void)
{
	return 0;
}

void DDuplexAudio::SetInVolume(tUInt32 val)
{
}
tUInt32 DDuplexAudio::GetInVolume(void)
{
	return 0;
}
string DDuplexAudio::GetWarning(void)
{
	string warning = _warning;
	_warning = "";		// and reset it,for warning is fetch only once.
	return warning;
}
void DDuplexAudio::Execute(Thread::Arg arg)
{
	DWORD dwResult;
	HRESULT hr;

	LOGINFO("DDuplexAudio Starting up ...");

	ResetCapture();
	ResetPlay();
	FillBufferWithSound(TRUE);	//full it first with silence first.

	if (FAILED(hr = StartCapture()) || FAILED(hr = StartPlay()))
	{
		LOGWARNING("DDuplexAudio::Execute:Failed to start Capturing or Playing,shutting down...");
		Shutdown();
	}

	while (!_is_shutdown_requested)
	{
		dwResult = MsgWaitForMultipleObjects( 2, &_events[0], 
                                              FALSE, INFINITE, QS_ALLEVENTS );

		switch( dwResult )
        {
            case WAIT_OBJECT_0 + 0:
                // rendering notification is signaled
                // This means that DirectSound just finished playing 
                // a piece of the buffer, so we need to fill the circular 
                // buffer with new sound from the wav file

                if( FAILED( hr = HandleRenderNotification() ) )
                {
                    //DXTRACE_ERR( TEXT("RecordCapturedData"), hr );
                    //MessageBox( NULL, TEXT("Error handling DirectSound notifications. Sample will now exit."), TEXT("DirectSound Sample"), 
                    //           MB_OK | MB_ICONERROR );
					LOGDEBUG("DDuplexAudio::Execute:Failed to HandleRenderNotification,shutting down...");
                    _is_shutdown_requested = TRUE;
                }
                break;

			case WAIT_OBJECT_0 + 1:
				// capturing notification is signaled
                // This means that DirectSound just finished capturing 
                // a piece of the buffer, so we can copy the captured data from the recorder buffer.

                if( FAILED( hr = HandleCaptureNotification() ) )
                {
                    //DXTRACE_ERR( TEXT("RecordCapturedData"), hr );
                    //MessageBox( NULL, TEXT("Error handling DirectSound notifications. Sample will now exit."), TEXT("DirectSound Sample"), 
                    //           MB_OK | MB_ICONERROR );
					LOGDEBUG("DDuplexAudio::Execute:Failed to HandleCaptureNotification,shutting down...");
                    _is_shutdown_requested = TRUE;
                }
                break;

            case WAIT_OBJECT_0 + 2:
                //// Windows messages are available
                //while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
                //{ 
                //    if( !IsDialogMessage( hDlg, &msg ) )  
                //    {
                //        TranslateMessage( &msg ); 
                //        DispatchMessage( &msg ); 
                //    }

                //    if( msg.message == WM_QUIT )
                //        bDone = TRUE;
			default :
				break;
		}
	}

	StopCapture();
	StopPlay();

	LOGINFO("DDuplexAudio shutdowned.");
}
 DDuplexAudio::DDuplexAudio(const DDuplexAudio& model)
 {}
tBool DDuplexAudio::Init()
{
	HRESULT hr;
	WAVEFORMATEX wfxPlay;
	WAVEFORMATEX wfxCapture;
	DSBUFFERDESC pcDSBufferDesc;
	DSCEFFECTDESC effectDesc[2];
	DSCBUFFERDESC pcDSCBufferDesc;

	IDirectSoundCaptureFXAec8* pDSAEC=NULL;
	IDirectSoundCaptureFXNoiseSuppress8* pDSNS=NULL;

	DSBPOSITIONNOTIFY*  aPosNotify0=NULL;
	DSBPOSITIONNOTIFY*  aPosNotify1=NULL;
	LPDIRECTSOUNDNOTIFY pDSNotify0=NULL;
	LPDIRECTSOUNDNOTIFY pDSNotify1=NULL;

	// Initialize COM
    if( FAILED( hr = CoInitialize(NULL) ) )
        return FALSE;

    ZeroMemory( &wfxPlay, sizeof(WAVEFORMATEX) ); 
    wfxPlay.wFormatTag      = WAVE_FORMAT_PCM; 
    wfxPlay.nChannels       = CHANNEL_NB; 
    wfxPlay.nSamplesPerSec  = SAMPLE_RATE;
    wfxPlay.wBitsPerSample  = BITS_PER_SAMPLE; 
    wfxPlay.nBlockAlign     = (WORD)(wfxPlay.wBitsPerSample / 8 * wfxPlay.nChannels);
    wfxPlay.nAvgBytesPerSec = (WORD)(wfxPlay.nSamplesPerSec * wfxPlay.nBlockAlign);

	ZeroMemory( &wfxCapture, sizeof(WAVEFORMATEX) ); 
    wfxCapture.wFormatTag      = WAVE_FORMAT_PCM; 
    wfxCapture.nChannels       = CHANNEL_NB; 
    wfxCapture.nSamplesPerSec  = SAMPLE_RATE;
    wfxCapture.wBitsPerSample  = BITS_PER_SAMPLE; 
    wfxCapture.nBlockAlign     = (WORD)(wfxCapture.wBitsPerSample / 8 * wfxCapture.nChannels);
    wfxCapture.nAvgBytesPerSec = (WORD)(wfxCapture.nSamplesPerSec * wfxCapture.nBlockAlign);

	ZeroMemory(   &pcDSBufferDesc,   sizeof(   pcDSBufferDesc   )   );
	pcDSBufferDesc.dwSize   =   sizeof(   pcDSBufferDesc   );
	pcDSBufferDesc.dwFlags   =  DSBCAPS_LOCDEFER | 
								DSBCAPS_CTRLPOSITIONNOTIFY | 
								DSBCAPS_GETCURRENTPOSITION2|
								DSBCAPS_GLOBALFOCUS;
	pcDSBufferDesc.dwBufferBytes   =   wfxPlay.nAvgBytesPerSec*DURATION/1000*PLAYER_BLOCK_NB;
	pcDSBufferDesc.dwReserved   =   0;
	pcDSBufferDesc.lpwfxFormat   =   &wfxPlay;
	pcDSBufferDesc.guid3DAlgorithm   =   DS3DALG_DEFAULT; 


	ZeroMemory(   &effectDesc[0],   sizeof(   DSCEFFECTDESC   )   );
	effectDesc[0].dwSize   =   sizeof(   DSCEFFECTDESC   );
	effectDesc[0].dwFlags   =   DSCFX_LOCSOFTWARE;
	effectDesc[0].guidDSCFXClass   =   GUID_DSCFX_CLASS_AEC;
	//effectDesc[0].guidDSCFXInstance   =   GUID_DSCFX_MS_AEC;
	effectDesc[0].guidDSCFXInstance   =   GUID_DSCFX_SYSTEM_AEC;
	effectDesc[0].dwReserved1   =   0;
	effectDesc[0].dwReserved2   =   0;

	ZeroMemory(   &effectDesc[1],   sizeof(   DSCEFFECTDESC   )   );
	effectDesc[1].dwSize   =   sizeof(   DSCEFFECTDESC   );
	effectDesc[1].dwFlags   =   DSCFX_LOCSOFTWARE;
	effectDesc[1].guidDSCFXClass   =   GUID_DSCFX_CLASS_NS;
	//effectDesc[1].guidDSCFXInstance   =   GUID_DSCFX_MS_NS;
	effectDesc[1].guidDSCFXInstance   =   GUID_DSCFX_SYSTEM_NS;
	effectDesc[1].dwReserved1   =   0;
	effectDesc[1].dwReserved2   =   0; 
	
	ZeroMemory(   &pcDSCBufferDesc,   sizeof(   pcDSCBufferDesc   )   );
	pcDSCBufferDesc.dwSize   =   sizeof(   pcDSCBufferDesc   );
	pcDSCBufferDesc.dwFlags   = DSCBCAPS_CTRLFX | 
								DSCBCAPS_WAVEMAPPED;
	pcDSCBufferDesc.dwBufferBytes   =   wfxCapture.nAvgBytesPerSec*DURATION/1000*RECORDER_BLOCK_NB;
	pcDSCBufferDesc.dwReserved   =   0;
	pcDSCBufferDesc.lpwfxFormat   =   &wfxCapture;
	pcDSCBufferDesc.dwFXCount   =   sizeof(effectDesc)/sizeof(effectDesc[0]);
	pcDSCBufferDesc.lpDSCFXDesc   =   effectDesc; 

	
	hr = DirectSoundFullDuplexCreate8(NULL,NULL,&pcDSCBufferDesc,&pcDSBufferDesc,m_hWnd,DSSCL_NORMAL,
											&m_pDSFD,&m_pDSCB,&m_pDSB,NULL);

	if (FAILED(hr))
	{
		/// 
		// that maybe other application is using directsound without AEC,which cause Here with AEC failed,so 
		// try Create it without AEC.
		_warning = "回声消除初始化失败,请关闭其他音频程序后重试!";

		pcDSCBufferDesc.dwFXCount   =   0;
		pcDSCBufferDesc.lpDSCFXDesc   =  0; 
		hr = DirectSoundFullDuplexCreate8(NULL,NULL,&pcDSCBufferDesc,&pcDSBufferDesc,m_hWnd,DSSCL_NORMAL,
											&m_pDSFD,&m_pDSCB,&m_pDSB,NULL);
		if (FAILED(hr))
			return FALSE;
	}

#ifdef DSOUND_AEC
	hr = m_pDSCB->GetObjectInPath(effectDesc[0].guidDSCFXClass,0,IID_IDirectSoundCaptureFXAec8,(LPVOID*)&pDSAEC);
	if (SUCCEEDED(hr))
	{
		DSCFXAec   parAEC;
		parAEC.fEnable = TRUE;
		parAEC.fNoiseFill   =   TRUE;
		parAEC.dwMode   =   DSCFX_AEC_MODE_FULL_DUPLEX;
		hr   =   pDSAEC->SetAllParameters(   &parAEC   );
	}

	//Noise suppression can be applied only if AEC is also enabled
	hr   =   m_pDSCB->GetObjectInPath(   effectDesc[1].guidDSCFXClass,0,IID_IDirectSoundCaptureFXNoiseSuppress8,(LPVOID*)&pDSNS);
	if (SUCCEEDED(hr))
	{
		DSCFXNoiseSuppress   parNS;
		parNS.fEnable   =   TRUE;
		hr   =   pDSNS->SetAllParameters(   &parNS   ); 
	}
	SAFE_RELEASE(pDSAEC);
	SAFE_RELEASE(pDSNS);
#endif

	m_dwNotifySize = wfxPlay.nAvgBytesPerSec*DURATION/1000;
	m_dwCaptureBufferSize = m_dwNotifySize*PLAYER_BLOCK_NB;
	m_dwWriteBufferSize = m_dwNotifySize*RECORDER_BLOCK_NB;

	///
	// Notifications for both rendering and capturing initialization
	// Create the notification events, so that we know when to fill
    // the buffer as the sound plays.
	_events[0] = CreateEvent( NULL, FALSE, FALSE, NULL );
	_events[1] = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( FAILED( hr = m_pDSB->QueryInterface( IID_IDirectSoundNotify, (VOID**)&pDSNotify0 ) ) )
		return FALSE;
    aPosNotify0 = new DSBPOSITIONNOTIFY[ PLAYER_BLOCK_NB ];
    for( DWORD i = 0; i < PLAYER_BLOCK_NB; i++ )
    {
        aPosNotify0[i].dwOffset     = (m_dwNotifySize * i) + m_dwNotifySize - 1;
        aPosNotify0[i].hEventNotify = _events[0];
    }
    if( FAILED( hr = pDSNotify0->SetNotificationPositions( PLAYER_BLOCK_NB, 
                                                          aPosNotify0 ) ) )
    {
        SAFE_RELEASE( pDSNotify0 );
        SAFE_DELETE( aPosNotify0 );
        return FALSE;
    }

    if( FAILED( hr = m_pDSCB->QueryInterface( IID_IDirectSoundNotify, (VOID**)&pDSNotify1 ) ) )
		return FALSE;
    aPosNotify1 = new DSBPOSITIONNOTIFY[ RECORDER_BLOCK_NB ];
    for( DWORD i = 0; i < RECORDER_BLOCK_NB; i++ )
    {
        aPosNotify1[i].dwOffset     = (m_dwNotifySize * i) + m_dwNotifySize - 1;
        aPosNotify1[i].hEventNotify = _events[1];
    }
    if( FAILED( hr = pDSNotify1->SetNotificationPositions( PLAYER_BLOCK_NB, 
                                                          aPosNotify1 ) ) )
    {
        SAFE_RELEASE( pDSNotify1 );
        SAFE_DELETE( aPosNotify1 );
        return FALSE;
    }

	SAFE_DELETE(aPosNotify0);
	SAFE_DELETE(aPosNotify1);
	SAFE_RELEASE(pDSNotify0);
	SAFE_RELEASE(pDSNotify1);

	_converter = new SpeexConverter();

	///
	// set circular buffers for render and capture
	m_pBufferPlayer = new DataBuffer(m_dwNotifySize,TOPLAY_BLOCK_NB);
	m_pBufferRecorder = new DataBuffer(m_dwNotifySize,RECORDER_BLOCK_NB);

	return TRUE;
}
tBool DDuplexAudio::Uninit()
{
	SAFE_RELEASE(m_pDSB);
	SAFE_RELEASE(m_pDSCB);
	SAFE_RELEASE(m_pDSFD);

	CloseHandle(_events[0]);
	CloseHandle(_events[1]);

	CoUninitialize();

	return TRUE;
}
void DDuplexAudio::ResetPlay()
{
	m_dwNextWriteOffset = 0;

	if (m_pDSB!=NULL)
		m_pDSB->SetCurrentPosition( 0L );

	if (m_pBufferPlayer!=NULL)
		m_pBufferPlayer->Empty();
}
void DDuplexAudio::ResetCapture()
{
	m_dwNextWriteOffset = 0;

	if (m_pDSB!=NULL)
		m_pDSB->SetCurrentPosition( 0L );

	if (m_pBufferRecorder!=NULL)
		m_pBufferRecorder->Empty();
}
HRESULT DDuplexAudio::StartPlay()
{
	HRESULT hr;

	if (m_pDSB==NULL)
		return S_FALSE;

	if( FAILED( hr = m_pDSB->Play( 0, 0, DSBPLAY_LOOPING ))  )
		return S_FALSE;

	return S_OK;
}
HRESULT DDuplexAudio::StartCapture()
{
	HRESULT hr;

	if (m_pDSCB==NULL)
		return S_FALSE;

	if( FAILED( hr = m_pDSCB->Start( DSCBSTART_LOOPING ) ) )
		return S_FALSE;

	return S_OK;
}
void DDuplexAudio::StopPlay()
{
	if (m_pDSB)
		m_pDSB->Stop();
}
void DDuplexAudio::StopCapture()
{
	if (m_pDSCB)
		m_pDSCB->Stop();
}
HRESULT DDuplexAudio::HandleRenderNotification(void)
{
	HRESULT hr;
	BOOL bRestored;

    if( FAILED( hr = RestoreBuffer( &bRestored ) ) )
		return hr;

	if( bRestored )
    	hr = FillBufferWithSound( TRUE );	//full it with data as it's restored
	else
		hr = FillBufferWithSound(FALSE);			// add one frame into it for play

	return hr;
}
HRESULT DDuplexAudio::HandleCaptureNotification(void)
{
	return RecordCapturedData();
}
HRESULT DDuplexAudio::RecordCapturedData() 
{
    HRESULT hr;
    VOID*   pbCaptureData    = NULL;
    DWORD   dwCaptureLength;
    VOID*   pbCaptureData2   = NULL;
    DWORD   dwCaptureLength2;
    VOID*   pbPlayData       = NULL;
    DWORD   dwReadPos;
    DWORD   dwCapturePos;
    LONG lLockSize;
	BYTE	aServalFrame[4*BLOCK_SIZE]={0};
	BYTE	aFrame[BLOCK_SIZE];
	short	asFrame[FRAME_SIZE];

    if( NULL == m_pDSCB )
        return S_FALSE;
    if( FAILED( hr = m_pDSCB->GetCurrentPosition( &dwCapturePos, &dwReadPos ) ) )
		return S_FALSE;

    lLockSize = dwReadPos - m_dwNextCaptureOffset;
    if( lLockSize < 0 )
        lLockSize += m_dwCaptureBufferSize;

    // Block align lock size so that we are always write on a boundary
    lLockSize -= (lLockSize % m_dwNotifySize);

    if( lLockSize == 0 )
        return S_FALSE;

    // Lock the capture buffer down
    if( FAILED( hr = m_pDSCB->Lock( m_dwNextCaptureOffset, lLockSize, 
                                          &pbCaptureData, &dwCaptureLength, 
                                          &pbCaptureData2, &dwCaptureLength2, 0L ) ) )
		return S_FALSE;

	//aServalFrame = new BYTE[lLockSize];

	memcpy(aServalFrame,pbCaptureData,dwCaptureLength);

    // Move the capture offset along
    m_dwNextCaptureOffset += dwCaptureLength; 
    m_dwNextCaptureOffset %= m_dwCaptureBufferSize; // Circular buffer

    if( pbCaptureData2 != NULL )
    {
		memcpy(aServalFrame+dwCaptureLength,pbCaptureData2,dwCaptureLength2);

        // Move the capture offset along
        m_dwNextCaptureOffset += dwCaptureLength2; 
        m_dwNextCaptureOffset %= m_dwCaptureBufferSize; // Circular buffer
    }

    // Unlock the capture buffer
    m_pDSCB->Unlock( pbCaptureData,  dwCaptureLength, 
                           pbCaptureData2, dwCaptureLength2 );

	for (int i=0;i<lLockSize/BLOCK_SIZE;i++)
	{
#ifdef SPEEX_AEC
		_converter->SpeexEchoCapture(&aServalFrame[i*BLOCK_SIZE],asFrame);
		memcpy(&aServalFrame[i*BLOCK_SIZE],asFrame,BLOCK_SIZE);
#endif SPEEX_AEC

#ifdef SPEEX_PREPROCESS
		_converter->SpeexPreprocessRun((short*)&aServalFrame[i*BLOCK_SIZE]);
#endif

		LOGDEBUG("DDuplexAudio::RecordCapturedData:Captured "<<BLOCK_SIZE<<" bytes.");

		m_pBufferRecorder->WriteData(&aServalFrame[i*BLOCK_SIZE],BLOCK_SIZE);
	}

	//if (aServalFrame) { delete[] aServalFrame; aServalFrame = NULL; }	

    return S_OK;
}

HRESULT DDuplexAudio::FillBufferWithSound(BOOL bResetAndFillAll)
{
	HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    
    VOID*   pDSLockedBuffer2   = NULL;
    DWORD   dwDSLockedBufferSize2 = 0;
    DWORD   dwPlayPos;
    DWORD   dwWritePos;
    LONG	lLockSize;
	DWORD	dwReadSize;
	DWORD	dwReadSizeAll=0;
	BYTE*	aServalFrame = NULL;
	BYTE	aFrame[BLOCK_SIZE];
	short	asFrame[FRAME_SIZE];    

    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;

	if( FAILED( hr = m_pDSB->GetCurrentPosition( &dwPlayPos, &dwWritePos ) ) )
        return S_FALSE;

    lLockSize = dwPlayPos - m_dwNextWriteOffset;
    if( lLockSize < 0 )
        lLockSize += m_dwWriteBufferSize;

    // Block align lock size so that we are always write on a boundary
    lLockSize -= (lLockSize % m_dwNotifySize);

	if (bResetAndFillAll)
	{
		LOGDEBUG("DDuplexAudio::FillBufferWithSound:Fill the whole buffer.");

		// reset 
		m_dwNextWriteOffset = 0;
		m_pDSB->SetCurrentPosition( 0L ); 

		while (m_dwNextWriteOffset<m_dwWriteBufferSize)
		{
			m_dwNextWriteOffset %= m_dwWriteBufferSize; // Circular buffer

			// Lock the buffer down
			if( FAILED( hr = m_pDSB->Lock( m_dwNextWriteOffset, m_dwNotifySize, 
										 &pDSLockedBuffer, &dwDSLockedBufferSize, 
										 NULL, NULL, 0L ) ) )
				return S_FALSE;

			dwReadSize = m_pBufferPlayer->ReadBlock((BYTE*)pDSLockedBuffer,m_dwNotifySize);

			if( dwReadSize < m_dwNotifySize )
			{
				// Wav is blank, so just fill with silence
				FillMemory( (BYTE*) pDSLockedBuffer, 
							dwDSLockedBufferSize, 
							(BYTE)0 );
			}

			// Unlock the buffer, we don't need it anymore.
			m_pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

			m_dwNextWriteOffset += dwDSLockedBufferSize; 
		}

		m_dwNextWriteOffset %= m_dwWriteBufferSize; // Circular buffer
	}
	else
	{
		if (lLockSize==0)
		return S_OK;

		// Lock the buffer down
		if( FAILED( hr = m_pDSB->Lock( m_dwNextWriteOffset, lLockSize, 
									 &pDSLockedBuffer, &dwDSLockedBufferSize, 
									 &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L ) ) )
			return S_FALSE;

		aServalFrame = new BYTE[lLockSize];

		while (dwReadSizeAll<lLockSize && (dwReadSize = m_pBufferPlayer->ReadBlock((BYTE*)(aServalFrame+dwReadSizeAll),m_dwNotifySize)))
			dwReadSizeAll += dwReadSize;

		if (dwReadSizeAll<lLockSize)
		{
			LOGWARNING("DDuplexAudio::FillBufferWithSound:Fill with silence "<<lLockSize-dwReadSizeAll<<"/"<<lLockSize<<" bytes.");
			// Wav is blank, so just fill with silence
			FillMemory( (BYTE*) (aServalFrame+dwReadSizeAll), 
						lLockSize-dwReadSizeAll, 
						(BYTE)0 );
			dwReadSizeAll = lLockSize;
		}

		CopyMemory(pDSLockedBuffer,aServalFrame,dwDSLockedBufferSize);
		if (pDSLockedBuffer2)
			CopyMemory(pDSLockedBuffer2,aServalFrame+dwDSLockedBufferSize,dwDSLockedBufferSize2);

		// Unlock the buffer, we don't need it anymore.
		m_pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );
		if (pDSLockedBuffer2)
			m_pDSB->Unlock( pDSLockedBuffer2, dwDSLockedBufferSize2, NULL, 0 );

		m_dwNextWriteOffset += lLockSize; 
		m_dwNextWriteOffset %= m_dwWriteBufferSize; // Circular buffer

		if (aServalFrame) { delete aServalFrame; aServalFrame = NULL; }
	}

	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: CStreamingSound::RestoreBuffer()
// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//       restored.  It can also be NULL if the information is not needed.
//-----------------------------------------------------------------------------
HRESULT DDuplexAudio::RestoreBuffer( BOOL* pbWasRestored )
{
    HRESULT hr;

    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = m_pDSB->GetStatus( &dwStatus ) ) )
        return hr;

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then DirectSound 
        // may not be giving us control yet, so restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = m_pDSB->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( hr != DS_OK );

        if( pbWasRestored != NULL )
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}
void DDuplexAudio::GetMergedFramFromJitters(tByte* buf,tUInt32 bufsize)
{
	short frame[FRAME_SIZE]={0};

	_ASSERT(bufsize>=FRAME_SIZE*2);

	memset(buf,0x00,bufsize);

	tFromJitterMapIter iter = _from_jitter_map.begin();

	while (iter != _from_jitter_map.end())
	{
		LOGDEBUG("DDuplexAudio::GetMergedFramFromJitters:Get a fram from the Jitter.");

		speex_jitter_get(iter->second,frame,NULL);

		//MixVoice((short*)buf,frame,FRAME_SIZE);
		memcpy(buf,frame,BLOCK_SIZE);
		break;

		iter++;
	}
}
void DDuplexAudio::MixVoice(tInt16* voice1,const tInt16* voice2,tUInt32 len)
{
	for (tUInt32 i=0;i<len;i++)
	{
		*(voice1+i) = *(voice1+i) + *(voice2+i);
	}
}