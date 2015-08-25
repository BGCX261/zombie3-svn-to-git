//-----------------------------------------------------------------------------
// File: DSUtil.cpp
//
// Desc: DirectSound framework classes for reading and writing wav files and
//       playing them in DirectSound buffers. Feel free to use this class 
//       as a starting point for adding extra functionality.
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <mmsystem.h>
#include <dxerr8.h>
#include <dsound.h>
#include "config.hxx"
#include "priv/DSUtil.h"
#include "priv/DXUtil.h"
#include "myfilelog.hxx"
#include "loghelper.hxx"


//-----------------------------------------------------------------------------
// Name: CSoundManager::CSoundManager()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSoundManager::CSoundManager()
:m_dwPrimaryChannels(0),
 m_dwPrimaryFreq(0),
 m_dwPrimaryBitRate(0)
{
    m_pDS = NULL;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::~CSoundManager()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
    SAFE_RELEASE( m_pDS ); 
}

//-----------------------------------------------------------------------------
// Name: CSoundManager::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Initialize( HWND  hWnd, 
                                   DWORD dwCoopLevel, 
                                   DWORD dwPrimaryChannels, 
                                   DWORD dwPrimaryFreq, 
                                   DWORD dwPrimaryBitRate )
{
    HRESULT             hr;

    SAFE_RELEASE( m_pDS );

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
        return DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

    // Set DirectSound coop level 
    if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
        return DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );
    
    // Set primary buffer format
    if( FAILED( hr = SetPrimaryBufferFormat( dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate ) ) )
        return DXTRACE_ERR( TEXT("SetPrimaryBufferFormat"), hr );

	m_dwPrimaryChannels = dwPrimaryChannels;
	m_dwPrimaryFreq = dwPrimaryFreq;
	m_dwPrimaryBitRate = dwPrimaryBitRate;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format 
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050, 
//               dwPrimaryBitRate  = 16
//-----------------------------------------------------------------------------
HRESULT CSoundManager::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                               DWORD dwPrimaryFreq, 
                                               DWORD dwPrimaryBitRate )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = dwPrimaryFreq;
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return DXTRACE_ERR( TEXT("SetFormat"), hr );

    SAFE_RELEASE( pDSBPrimary );

	m_dwPrimaryChannels = dwPrimaryChannels;
	m_dwPrimaryFreq = dwPrimaryFreq;
	m_dwPrimaryBitRate = dwPrimaryBitRate;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Get3DListenerInterface()
// Desc: Returns the 3D listener interface associated with primary buffer.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener )
{
    HRESULT             hr;
    DSBUFFERDESC        dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( ppDSListener == NULL )
        return E_INVALIDARG;
    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    *ppDSListener = NULL;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)ppDSListener ) ) )
    {
        SAFE_RELEASE( pDSBPrimary );
        return DXTRACE_ERR( TEXT("QueryInterface"), hr );
    }

    // Release the primary buffer, since it is not need anymore
    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateStreaming()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateStreaming( CStreamingSound** ppStreamingSound,
                                        DWORD dwCreationFlags, 
                                        GUID guid3DAlgorithm,
                                        DWORD dwNotifyCount, 
                                        DWORD m_dwNotifySize, 
                                        HANDLE hNotifyEvent,
										FileLog* filelog)
{
    HRESULT hr;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;
    if( ppStreamingSound == NULL || hNotifyEvent == NULL )
        return E_INVALIDARG;

    LPDIRECTSOUNDBUFFER pDSBuffer      = NULL;
    DWORD               dwDSBufferSize = NULL;
    DSBPOSITIONNOTIFY*  aPosNotify     = NULL; 
    LPDIRECTSOUNDNOTIFY m_pDSNotify      = NULL;

    // Figure out how big the DSound buffer should be 
    dwDSBufferSize = m_dwNotifySize * dwNotifyCount;

    // Set up the direct sound buffer.  Request the NOTIFY flag, so
    // that we are notified as the sound buffer plays.  Note, that using this flag
    // may limit the amount of hardware acceleration that can occur. 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags | 
                           DSBCAPS_CTRLPOSITIONNOTIFY | 
						   DSBCAPS_GETCURRENTPOSITION2|
						   DSBCAPS_GLOBALFOCUS | 
						   DSBCAPS_LOCSOFTWARE;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;

	//LPWAVEFORMATEX wfx = new WAVEFORMATEX;
	WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) m_dwPrimaryChannels; 
    wfx.nSamplesPerSec  = m_dwPrimaryFreq;
    wfx.wBitsPerSample  = (WORD) m_dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    dsbd.lpwfxFormat     = &wfx; 

    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBuffer, NULL ) ) )
    {
        // If wave format isn't then it will return 
        // either DSERR_BADFORMAT or E_INVALIDARG
        if( hr == DSERR_BADFORMAT || hr == E_INVALIDARG )
            return DXTRACE_ERR_NOMSGBOX( TEXT("CreateSoundBuffer"), hr );

        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
    }

    // Create the notification events, so that we know when to fill
    // the buffer as the sound plays. 
    if( FAILED( hr = pDSBuffer->QueryInterface( IID_IDirectSoundNotify, 
                                                (VOID**)&m_pDSNotify ) ) )
    {
        SAFE_DELETE( aPosNotify );
        return DXTRACE_ERR( TEXT("QueryInterface"), hr );
    }

    aPosNotify = new DSBPOSITIONNOTIFY[ dwNotifyCount ];
    if( aPosNotify == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < dwNotifyCount; i++ )
    {
        aPosNotify[i].dwOffset     = (m_dwNotifySize * i) + m_dwNotifySize - 1;
        aPosNotify[i].hEventNotify = hNotifyEvent;             
    }
    
    // Tell DirectSound when to notify us. The notification will come in the from 
    // of signaled events that are handled in WinMain()
    if( FAILED( hr = m_pDSNotify->SetNotificationPositions( dwNotifyCount, 
                                                          aPosNotify ) ) )
    {
        SAFE_RELEASE( m_pDSNotify );
        SAFE_DELETE( aPosNotify );
        return DXTRACE_ERR( TEXT("SetNotificationPositions"), hr );
    }

    SAFE_RELEASE( m_pDSNotify );
    SAFE_DELETE( aPosNotify );

    // Create the sound
    *ppStreamingSound = new CStreamingSound( pDSBuffer, dwDSBufferSize, m_dwNotifySize,hNotifyEvent,filelog);

    return S_OK;
}

HRESULT CStreamingSound::FillBufferWithSound(BOOL restored)
{
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    
    VOID*   pDSLockedBuffer2   = NULL;
    DWORD   dwDSLockedBufferSize2 = 0;
    //UINT    dwDataWrote;
    DWORD   dwPlayPos;
    DWORD   dwWritePos;
    LONG	lLockSize;
	DWORD	dwReadSize;
	DWORD	dwReadSizeAll=0;
	BYTE*	aServalFrame = NULL;
	BYTE	aFrame[BLOCK_SIZE];
	//BYTE	aFrame2[BLOCK_SIZE];
	short	asFrame[FRAME_SIZE];    

    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;

	if( FAILED( hr = m_pDSB->GetCurrentPosition( &dwPlayPos, &dwWritePos ) ) )
        return DXTRACE_ERR( TEXT("GetCurrentPosition"), hr );

    lLockSize = dwPlayPos - m_dwNextWriteOffset;
    if( lLockSize < 0 )
        lLockSize += m_dwWriteBufferSize;

    // Block align lock size so that we are always write on a boundary
    lLockSize -= (lLockSize % m_dwNotifySize);

	if (lLockSize==0)
		return S_OK;

	if (restored)
	{
		Reset();
		
		while (m_dwNextWriteOffset<m_dwWriteBufferSize)
		{
			m_dwNextWriteOffset %= m_dwWriteBufferSize; // Circular buffer

			// Lock the buffer down
			if( FAILED( hr = m_pDSB->Lock( m_dwNextWriteOffset, m_dwNotifySize, 
										 &pDSLockedBuffer, &dwDSLockedBufferSize, 
										 NULL, NULL, 0L ) ) )
				return DXTRACE_ERR( TEXT("Lock"), hr );

			dwReadSize = m_pDataBuffer->ReadBlock((BYTE*)pDSLockedBuffer,m_dwNotifySize);

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
		// Lock the buffer down
		if( FAILED( hr = m_pDSB->Lock( m_dwNextWriteOffset, lLockSize, 
									 &pDSLockedBuffer, &dwDSLockedBufferSize, 
									 &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L ) ) )
			return DXTRACE_ERR( TEXT("Lock"), hr );

		aServalFrame = new BYTE[lLockSize];

		while (dwReadSizeAll<lLockSize && (dwReadSize = m_pDataBuffer->ReadBlock((BYTE*)(aServalFrame+dwReadSizeAll),m_dwNotifySize)))
			dwReadSizeAll += dwReadSize;

		if (dwReadSizeAll<lLockSize)
		{
			LOGWARNING("CStreamingSound::FillBufferWithSound:Fill with silence "<<lLockSize-dwReadSizeAll<<"/"<<lLockSize<<" bytes.");
			// Wav is blank, so just fill with silence
			FillMemory( (BYTE*) (aServalFrame+dwReadSizeAll), 
						lLockSize-dwReadSizeAll, 
						(BYTE)0 );
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
HRESULT CStreamingSound::RestoreBuffer( BOOL* pbWasRestored )
{
    HRESULT hr;

    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = m_pDSB->GetStatus( &dwStatus ) ) )
        return DXTRACE_ERR( TEXT("GetStatus"), hr );

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




DWORD CStreamingSound::WriteSound(BYTE* pSound, DWORD dwSize)
{
	if (NULL==m_pDataBuffer)
		return 0;

	return m_pDataBuffer->WriteData(pSound,dwSize);
}
//-----------------------------------------------------------------------------
// Name: CStreamingSound::Get3DBufferInterface()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::Get3DBufferInterface(LPDIRECTSOUND3DBUFFER* ppDS3DBuffer )
{
    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;

    *ppDS3DBuffer = NULL;

    return m_pDSB->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)ppDS3DBuffer );
}


//-----------------------------------------------------------------------------
// Name: CStreamingSound::Play()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::Play( DWORD dwPriority, DWORD dwFlags )
{
    HRESULT hr;
    BOOL    bRestored;

    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;

    // Restore the buffer if it was lost
    if( FAILED( hr = RestoreBuffer( &bRestored ) ) )
        return DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // Make DirectSound do pre-processing on sound effects
        Reset();
    }

    return m_pDSB->Play( 0, dwPriority, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::Stop()
// Desc: Stops the sound from playing
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::Stop()
{
    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    hr |= m_pDSB->Stop();

    return hr;
}





BOOL CStreamingSound::IsSoundPlaying()
{
    BOOL bIsPlaying = FALSE;

    if( m_pDSB == NULL )
        return FALSE; 

    DWORD dwStatus = 0;
    m_pDSB->GetStatus( &dwStatus );
    bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );

    return bIsPlaying;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::CStreamingSound()
// Desc: Setups up a buffer so data can be streamed from the wave file into 
//       buffer.  This is very useful for large wav files that would take a 
//       while to load.  The buffer is initially filled with data, then 
//       as sound is played the notification events are signaled and more data
//       is written into the buffer by calling HandleWaveStreamNotification()
//-----------------------------------------------------------------------------
CStreamingSound::CStreamingSound( LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize,DWORD m_dwNotifySize ,HANDLE hEventHandle,FileLog* filelog)
{
	_ASSERT(0==dwDSBufferSize%m_dwNotifySize);

	m_pDataBuffer		= new DataBuffer(m_dwNotifySize,2*(dwDSBufferSize/m_dwNotifySize));
	m_pDSB			= pDSBuffer;
	m_dwWriteBufferSize	= dwDSBufferSize;
    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNotifySize      = m_dwNotifySize;
    m_dwNextWriteOffset = 0;

	m_hEventHandle		= hEventHandle;
	_filelog			= filelog;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::~CStreamingSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CStreamingSound::~CStreamingSound()
{
	delete m_pDataBuffer;
}



//-----------------------------------------------------------------------------
// Name: CStreamingSound::HandleWaveStreamNotification()
// Desc: Handle the notification that tell us to put more wav data in the 
//       circular buffer
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::HandleWaveStreamNotification()
{
    HRESULT hr;
    //DWORD   dwCurrentPlayPos;
    //DWORD   dwPlayDelta;
    //DWORD   dwBytesWrittenToBuffer;
    //VOID*   pDSLockedBuffer = NULL;
    //VOID*   pDSLockedBuffer2 = NULL;
    //DWORD   dwDSLockedBufferSize;
    //DWORD   dwDSLockedBufferSize2;

    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if( FAILED( hr = RestoreBuffer( &bRestored ) ) )
        return DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    	hr = FillBufferWithSound( TRUE );	//full it with data as it's restored
	else
		hr = FillBufferWithSound(FALSE);			// add one frame into it for play

    // The buffer was restored, so we need to fill it with new data
    if( FAILED( hr) )
        return DXTRACE_ERR( TEXT("FillBufferWithSound"), hr );

    return S_OK;
}




BOOL CStreamingSound::ReadyForPlayMore() const
{
	if (m_pDataBuffer==NULL)
		return FALSE;

	return m_pDataBuffer->ReadyForBufferMore();
}
const BYTE* CStreamingSound::GetLastPlayedFrame()
{
	HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

	// Lock the buffer down
	if( FAILED( hr = m_pDSB->Lock( m_dwNextWriteOffset, m_dwNotifySize, 
								 &pDSLockedBuffer, &dwDSLockedBufferSize, 
								 NULL, NULL, 0L ) ) )
		return NULL;

	m_pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );


	if( dwDSLockedBufferSize != m_dwNotifySize )
		return NULL;
	else
		return (BYTE*)pDSLockedBuffer;
}
const BYTE*	CStreamingSound::GetNextPlayFrame()
{
	HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 
	DWORD	dwNextPlayOffset	 = 0;

	dwNextPlayOffset = (m_dwNextWriteOffset + m_dwNotifySize) % m_dwWriteBufferSize;

	// Lock the buffer down
	if( FAILED( hr = m_pDSB->Lock( dwNextPlayOffset, m_dwNotifySize, 
								 &pDSLockedBuffer, &dwDSLockedBufferSize, 
								 NULL, NULL, 0L ) ) )
		return NULL;

	m_pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );


	if( dwDSLockedBufferSize != m_dwNotifySize )
		return NULL;
	else
		return (BYTE*)pDSLockedBuffer;
}
//-----------------------------------------------------------------------------
// Name: CStreamingSound::Reset()
// Desc: Resets the sound so it will begin playing at the beginning
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::Reset()
{
    HRESULT hr;

    if( m_pDSB == NULL )
        return CO_E_NOTINITIALIZED;

    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNextWriteOffset = 0;
    //m_bFillNextNotificationWithSilence = FALSE;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if( FAILED( hr = RestoreBuffer( &bRestored ) ) )
        return DXTRACE_ERR( TEXT("RestoreBuffer"), hr );

    return m_pDSB->SetCurrentPosition( 0L );  
}


DataBuffer::DataBuffer(DWORD dwBlockSize,DWORD dwBlockNum)
:m_apBuffer(NULL),
 m_dwBlockSize(dwBlockSize),
 m_dwBlockNum(dwBlockNum),
 m_dwFreeBlock(dwBlockNum),
 m_dwCurRead(0),
 m_dwCurWrite(0),
 m_dwCurWritePos(0),
 m_CurReadCSec()
{
	m_apBuffer = new BYTE*[dwBlockNum];

	for (DWORD i=0;i<dwBlockNum;i++)
	{
		m_apBuffer[i] = new BYTE[dwBlockSize];
	}
}
DataBuffer::~DataBuffer()
{
	if (m_apBuffer)
	{
		for (DWORD i=0;i<m_dwBlockNum;i++)
		{
			if (m_apBuffer[i]) delete[] m_apBuffer[i];
		}
		delete[] m_apBuffer;
	}
}

DWORD DataBuffer::WriteData(BYTE* data,DWORD dwSize)
{
	if (m_dwFreeBlock==0 || dwSize==0)
		return 0;

	if (m_dwBlockSize-m_dwCurWritePos > dwSize)
	{
		CopyMemory(m_apBuffer[m_dwCurWrite]+m_dwCurWritePos,data,dwSize);
		m_dwCurWritePos += dwSize;
		return dwSize;
	}
	else
	{
		DWORD dwFirstSize = m_dwBlockSize-m_dwCurWritePos;
		DWORD dwSecondSize = dwSize - dwFirstSize;

		CopyMemory(m_apBuffer[m_dwCurWrite]+m_dwCurWritePos,data,dwFirstSize);

		m_dwCurWrite++;
		m_dwCurWrite %= m_dwBlockNum;
		m_dwCurWritePos = 0;
		m_dwFreeBlock--;

		if (dwSecondSize>0)
			return dwFirstSize+WriteData(data+dwFirstSize,dwSecondSize);
		else 
			return dwFirstSize;
	}
}
DWORD DataBuffer::ReadBlock(BYTE* buf,DWORD dwBufSize)
{	
	if (m_dwFreeBlock==m_dwBlockNum)
		return 0;

	//_ASSERT(dwBufSize==m_dwBlockSize);

	DWORD dwReadSize = dwBufSize<m_dwBlockSize ? dwBufSize : m_dwBlockSize;

	CopyMemory(buf,m_apBuffer[m_dwCurRead],dwReadSize);
	m_dwCurRead++;
	m_dwCurRead %= m_dwBlockNum;
	m_dwFreeBlock++;

	return dwReadSize;
}
//DWORD DataBuffer::WriteAndJump(BYTE* data,DWORD dwSize)
//{
//	if (m_dwFreeBlock==0 || dwSize==0)
//		return 0;
//
//	DWORD dwFirstSize = m_dwBlockSize-m_dwCurWritePos;
//	DWORD dwSecondSize = dwSize - dwFirstSize;
//
//	CopyMemory(m_apBuffer[m_dwCurWrite],data,dwSize);
//
//	m_dwFreeBlock--;
//	m_dwCurWrite++;
//	m_dwCurWrite %= m_dwBlockNum;
//	m_dwCurWritePos = 0;
//
//	return dwSize;
//}
//DWORD DataBuffer::ReadAndJump(BYTE* buf,DWORD dwBufSize)
//{
//	if (m_dwFreeBlock==m_dwBlockNum)
//		return 0;
//
//	//_ASSERT(dwBufSize==m_dwBlockSize);
//
//	DWORD dwReadSize = dwBufSize<m_dwBlockSize ? dwBufSize : m_dwBlockSize;
//
//	CopyMemory(buf,m_apBuffer[m_dwCurRead],dwReadSize);
//	m_dwCurRead++;
//	m_dwCurRead %= m_dwBlockNum;
//	m_dwFreeBlock++;
//
//	return dwReadSize;
//}
BOOL DataBuffer::ReadyForBufferMore() const
{
	return (m_dwFreeBlock>0);
}
void DataBuffer::Empty()
{
	m_dwCurRead = 0;
	m_dwCurWrite = 0;
	m_dwCurWritePos = 0;
}