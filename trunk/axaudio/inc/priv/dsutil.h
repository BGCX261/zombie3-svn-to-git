//-----------------------------------------------------------------------------
// File: DSUtil.h
//
// Desc: 
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef DSUTIL_H
#define DSUTIL_H
#include <map>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

#include "mythread.hxx"
#include "mycriticalsection.hxx"


//-----------------------------------------------------------------------------
// Classes used by this header
//-----------------------------------------------------------------------------
class CSoundManager;
class CStreamingSound;
class DataBuffer;
class FileLog;



//-----------------------------------------------------------------------------
// Typing macros 
//-----------------------------------------------------------------------------
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

#define DSUtil_StopSound(s)         { if(s) s->Stop(); }
#define DSUtil_PlaySound(s)         { if(s) s->Play( 0, 0 ); }
#define DSUtil_PlaySoundLooping(s)  { if(s) s->Play( 0, DSBPLAY_LOOPING ); }




//-----------------------------------------------------------------------------
// Name: class CSoundManager
// Desc: 
//-----------------------------------------------------------------------------
class CSoundManager
{
protected:
    LPDIRECTSOUND8 m_pDS;

	//keep a track
	DWORD m_dwPrimaryChannels;
	DWORD m_dwPrimaryFreq;
	DWORD m_dwPrimaryBitRate; 

	

public:

    CSoundManager();
    ~CSoundManager();


    HRESULT Initialize( HWND hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    inline  LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
    HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    HRESULT Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );

    HRESULT CreateStreaming( CStreamingSound** ppStreamingSound, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNotifyCount, DWORD m_dwNotifySize, HANDLE hNotifyEvent,FileLog* filelog);
};

class CStreamingSound
{
protected:
	DataBuffer* m_pDataBuffer;

	LPDIRECTSOUNDBUFFER m_pDSB;
    DWORD               m_dwWriteBufferSize;

    DWORD m_dwLastPlayPos;
    DWORD m_dwPlayProgress;
    DWORD m_dwNotifySize;
    DWORD m_dwNextWriteOffset;

	HANDLE m_hEventHandle;
	FileLog* _filelog;

	HRESULT RestoreBuffer( BOOL* pbWasRestored );

public:
    CStreamingSound( LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, DWORD m_dwNotifySize,HANDLE hEventHandle,FileLog* filelog);
    ~CStreamingSound();

	DWORD	WriteSound(BYTE* pSound, DWORD dwSize);
    HRESULT FillBufferWithSound(BOOL restored);

    HRESULT Get3DBufferInterface(LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );

    HRESULT Play( DWORD dwPriority = 0, DWORD dwFlags = 0 );
    HRESULT Stop();
    HRESULT Reset();
    BOOL    IsSoundPlaying();

    HRESULT HandleWaveStreamNotification();
	BOOL	ReadyForPlayMore() const;
	const BYTE*	GetLastPlayedFrame();
	const BYTE*	GetNextPlayFrame();
};

class DataBuffer
{
public:
	DataBuffer(DWORD dwBlockSize,DWORD dwBlockNum);
	~DataBuffer();

	DWORD WriteData(BYTE* data,DWORD dwSize);
	DWORD ReadBlock(BYTE* buf,DWORD dwBufSize);

	//DWORD WriteAndJump(BYTE* data,DWORD dwSize);
	//DWORD ReadAndJump(BYTE* data,DWORD dwSize);

	BOOL ReadyForBufferMore() const;
	void Empty();

private:
	BYTE** m_apBuffer;
	DWORD m_dwBlockSize;
	DWORD m_dwBlockNum;
	DWORD m_dwFreeBlock;

	DWORD m_dwCurRead;
	DWORD m_dwCurWrite;
	DWORD m_dwCurWritePos;

	CriticalSection m_CurReadCSec;
};
#endif // DSUTIL_H
