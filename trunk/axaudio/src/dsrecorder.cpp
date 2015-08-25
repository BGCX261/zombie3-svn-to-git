#include "axaudioexception.hxx"
#include "priv/speexconverter.hxx"
#include "priv/dsrecorder.hxx"

DSRecorder::DSRecorder(void* hWnd,SpeexConverter* converter,FileLog* filelog)
:m_hWnd((HWND)hWnd),m_pDataBuffer(NULL),
 m_pDSCapture(NULL),
 m_pDSCB(NULL),
 m_pDSNotify(NULL),
 m_guidCaptureDevice(GUID_NULL),
 m_wfxInput(),
 m_dwNotifySize(BLOCK_SIZE),
 m_hNotificationEvent(NULL),
 m_dwCaptureBufferSize(NULL),
 m_dwNextCaptureOffset(0),
 _converter(converter),
 _is_shutdown_requested(FALSE),
 _filelog(filelog)
{
	if (!Init())
		throw AXAudioException("Failed to init DSRecorder!");
}
DSRecorder::~DSRecorder()
{
	Uninit();
}
void DSRecorder::Shutdown()
{
	_is_shutdown_requested = TRUE;
}
tUInt32 DSRecorder::ReadSound(tByte* buf, tUInt32 bufsize)
{
	return m_pDataBuffer->ReadBlock(buf,bufsize);
}
void DSRecorder::Execute(Thread::Arg)
{
	DWORD dwResult;
	HRESULT hr;

	StartOrStopRecord(TRUE);

	while (!_is_shutdown_requested)
	{
		dwResult = MsgWaitForMultipleObjects( 1, &m_hNotificationEvent, 
                                              FALSE, INFINITE, QS_ALLEVENTS );

		switch( dwResult )
        {
            case WAIT_OBJECT_0 + 0:
                // g_hNotificationEvents[0] is signaled

                // This means that DirectSound just finished playing 
                // a piece of the buffer, so we need to fill the circular 
                // buffer with new sound from the wav file

                if( FAILED( hr = RecordCapturedData() ) )
                {
                    DXTRACE_ERR( TEXT("RecordCapturedData"), hr );
                    MessageBox( NULL, TEXT("Error handling DirectSound notifications. Sample will now exit."), TEXT("DirectSound Sample"), 
                               MB_OK | MB_ICONERROR );
                    _is_shutdown_requested = TRUE;
                }
                break;

            case WAIT_OBJECT_0 + 1:
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

	// Stop the capture and read any data that was not caught by a notification
    StartOrStopRecord( FALSE );

    // Clean up everything
    //FreeDirectSound();
}
tBool DSRecorder::Init()
{
	HRESULT hr;

	// Initialize COM
    if( FAILED( hr = CoInitialize(NULL) ) )
        return DXTRACE_ERR( TEXT("CoInitialize"), hr );

	m_hNotificationEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	if (m_hNotificationEvent==NULL)
		return FALSE;

	if (FAILED(hr=InitDirectSound(m_hWnd,&m_guidCaptureDevice)))
		return FALSE;

	ZeroMemory( &m_wfxInput, sizeof(WAVEFORMATEX) ); 
    m_wfxInput.wFormatTag      = WAVE_FORMAT_PCM; 
    m_wfxInput.nChannels       = (WORD) CHANNEL_NB; 
    m_wfxInput.nSamplesPerSec  = SAMPLE_RATE;
    m_wfxInput.wBitsPerSample  = (WORD) BITS_PER_SAMPLE; 
    m_wfxInput.nBlockAlign     = (WORD) (m_wfxInput.wBitsPerSample / 8 * m_wfxInput.nChannels);
    m_wfxInput.nAvgBytesPerSec = m_wfxInput.nSamplesPerSec * m_wfxInput.nBlockAlign;

	// Create a capture buffer, and tell the capture 
    // buffer to start recording   
    if( FAILED( hr = CreateCaptureBuffer( &m_wfxInput ) ) )
        return FALSE;

	m_pDataBuffer = new DataBuffer(BLOCK_SIZE,RECORDED_BLOCK_NB);

	return TRUE;
}
tBool DSRecorder::Uninit()
{
	if (m_pDataBuffer) { delete m_pDataBuffer;m_pDataBuffer=NULL; }

	// Release DirectSound interfaces
    SAFE_RELEASE( m_pDSNotify );
    SAFE_RELEASE( m_pDSCB );
    SAFE_RELEASE( m_pDSCapture ); 

	CloseHandle(m_hNotificationEvent);

    // Release COM
    CoUninitialize();

	return TRUE;
}
HRESULT DSRecorder::InitDirectSound( HWND hDlg, GUID* pDeviceGuid )
{
    HRESULT hr;

    ZeroMemory( &m_aPosNotify, sizeof(DSBPOSITIONNOTIFY) * 
                               (RECORDER_BLOCK_NB + 1) );
    m_dwCaptureBufferSize = 0;
    m_dwNotifySize        = 0;

    // Create IDirectSoundCapture using the preferred capture device
    if( FAILED( hr = DirectSoundCaptureCreate( pDeviceGuid, &m_pDSCapture, NULL ) ) )
        return DXTRACE_ERR( TEXT("DirectSoundCaptureCreate"), hr );

    return S_OK;
}
HRESULT DSRecorder::CreateCaptureBuffer( WAVEFORMATEX* pwfxInput )
{
    HRESULT hr;
    DSCBUFFERDESC dscbd;

    SAFE_RELEASE( m_pDSNotify );
    SAFE_RELEASE( m_pDSCB );

    // Set the notification size
    //m_dwNotifySize = MAX( 1024, pwfxInput->nAvgBytesPerSec / 8 );
	m_dwNotifySize = (pwfxInput->nAvgBytesPerSec * DURATION)/ 1000;
    m_dwNotifySize -= m_dwNotifySize % pwfxInput->nBlockAlign;   

    // Set the buffer sizes 
    m_dwCaptureBufferSize = m_dwNotifySize * RECORDER_BLOCK_NB;

    // Create the capture buffer
    ZeroMemory( &dscbd, sizeof(dscbd) );
    dscbd.dwSize        = sizeof(dscbd);
    dscbd.dwBufferBytes = m_dwNotifySize * RECORDER_BLOCK_NB;
    dscbd.lpwfxFormat   = pwfxInput; // Set the format during creatation
	dscbd.dwFlags		= DSCBCAPS_CTRLFX | DSCBCAPS_WAVEMAPPED;
	dscbd.dwFXCount		= 2;

    if( FAILED( hr = m_pDSCapture->CreateCaptureBuffer( &dscbd, 
                                                        &m_pDSCB, 
                                                        NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateCaptureBuffer"), hr );


    if( FAILED( hr = InitNotifications() ) )
        return DXTRACE_ERR( TEXT("InitNotifications"), hr );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitNotifications()
// Desc: Inits the notifications on the capture buffer which are handled
//       in WinMain()
//-----------------------------------------------------------------------------
HRESULT DSRecorder::InitNotifications()
{
    HRESULT hr; 
	LPDIRECTSOUNDNOTIFY m_pDSNotify;

    if( NULL == m_pDSCB )
        return E_FAIL;

    // Create a notification event, for when the sound stops playing
    if( FAILED( hr = m_pDSCB->QueryInterface( IID_IDirectSoundNotify, 
                                                    (VOID**)&m_pDSNotify ) ) )
        return DXTRACE_ERR( TEXT("QueryInterface"), hr );

    // Setup the notification positions
    for( INT i = 0; i < RECORDER_BLOCK_NB; i++ )
    {
        m_aPosNotify[i].dwOffset = (m_dwNotifySize * i) + m_dwNotifySize - 1;
        m_aPosNotify[i].hEventNotify = m_hNotificationEvent;             
    }
    
    // Tell DirectSound when to notify us. the notification will come in the from 
    // of signaled events that are handled in WinMain()
    if( FAILED( hr = m_pDSNotify->SetNotificationPositions( RECORDER_BLOCK_NB, 
                                                            m_aPosNotify ) ) )
        return DXTRACE_ERR( TEXT("SetNotificationPositions"), hr );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: StartOrStopRecord()
// Desc: Starts or stops the capture buffer from recording
//-----------------------------------------------------------------------------
HRESULT DSRecorder::StartOrStopRecord( BOOL bStartRecording )
{
    HRESULT hr;

    if( bStartRecording )
    {
        if( FAILED( hr = m_pDSCB->Start( DSCBSTART_LOOPING ) ) )
            return DXTRACE_ERR( TEXT("Start"), hr );
    }
    else
    {
        // Stop the capture and read any data that 
        // was not caught by a notification
        if( NULL == m_pDSCB )
            return S_OK;

        // Stop the buffer, and read any data that was not 
        // caught by a notification
        if( FAILED( hr = m_pDSCB->Stop() ) )
            return DXTRACE_ERR( TEXT("Stop"), hr );

        if( FAILED( hr = RecordCapturedData() ) )
            return DXTRACE_ERR( TEXT("RecordCapturedData"), hr );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RecordCapturedData()
// Desc: Copies data from the capture buffer to the output buffer 
//-----------------------------------------------------------------------------
HRESULT DSRecorder::RecordCapturedData() 
{
    HRESULT hr;
    VOID*   pbCaptureData    = NULL;
    DWORD   dwCaptureLength;
    VOID*   pbCaptureData2   = NULL;
    DWORD   dwCaptureLength2;
    VOID*   pbPlayData       = NULL;
    //UINT    dwDataWrote;
    DWORD   dwReadPos;
    DWORD   dwCapturePos;
    LONG lLockSize;
	BYTE*	aServalFrame = NULL;
	BYTE	aFrame[BLOCK_SIZE];
	//BYTE	aFrame2[BLOCK_SIZE];
	short	asFrame[FRAME_SIZE];

    if( NULL == m_pDSCB )
        return S_FALSE;
    //if( NULL == g_pWaveFile )
    //    return S_FALSE;

    if( FAILED( hr = m_pDSCB->GetCurrentPosition( &dwCapturePos, &dwReadPos ) ) )
        return DXTRACE_ERR( TEXT("GetCurrentPosition"), hr );

    lLockSize = dwReadPos - m_dwNextCaptureOffset;
    if( lLockSize < 0 )
        lLockSize += m_dwCaptureBufferSize;

    // Block align lock size so that we are always write on a boundary
    lLockSize -= (lLockSize % m_dwNotifySize);

	//// Two blocks a time at most.
	//if (lLockSize>m_dwNotifySize*2)
	//	lLockSize = m_dwNotifySize*2;

    if( lLockSize == 0 )
        return S_FALSE;

    // Lock the capture buffer down
    if( FAILED( hr = m_pDSCB->Lock( m_dwNextCaptureOffset, lLockSize, 
                                          &pbCaptureData, &dwCaptureLength, 
                                          &pbCaptureData2, &dwCaptureLength2, 0L ) ) )
        return DXTRACE_ERR( TEXT("Lock"), hr );

	aServalFrame = new BYTE[lLockSize];

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
		_converter->SpeexPreprocessRun((short*)&aServalFrame[i*BLOCK_SIZE]);
		
		m_pDataBuffer->WriteData(&aServalFrame[i*BLOCK_SIZE],BLOCK_SIZE);
	}

	if (aServalFrame) { delete[] aServalFrame; aServalFrame = NULL; }	

    return S_OK;
}
