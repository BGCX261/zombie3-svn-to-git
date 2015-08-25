#pragma once

#include <windows.h>
#include <basetsd.h>
#include <commdlg.h>
#include <mmreg.h>
#include <dxerr8.h>
#include <dsound.h>
#include "DSUtil.h"
#include "config.hxx"
#include "mythread.hxx"

class SpeexConverter;
class FileLog;

//#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); delete (p);(p)=NULL; } }
//#define MAX(a,b) (a)>(b) ? (a) : (b)
#define MAX(a,b)        ( (a) > (b) ? (a) : (b) )

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

class DSRecorder : public Thread
{
public:
	DSRecorder(void* hWnd,SpeexConverter* converter,FileLog* filelog);
	~DSRecorder();

	void Shutdown();

	tUInt32 ReadSound(tByte* buf, tUInt32 bufsize);

protected:
	virtual void Execute(Thread::Arg);

private:
	tBool Init();
	tBool Uninit();

	HRESULT InitDirectSound( HWND hDlg, GUID* pDeviceGuid );
	HRESULT CreateCaptureBuffer( WAVEFORMATEX* pwfxInput );
	HRESULT InitNotifications();
	HRESULT StartOrStopRecord( BOOL bStartRecording );
	HRESULT RecordCapturedData();

	HWND m_hWnd;
	DataBuffer* m_pDataBuffer;

	LPDIRECTSOUNDCAPTURE       m_pDSCapture;
	LPDIRECTSOUNDCAPTUREBUFFER m_pDSCB;
	LPDIRECTSOUNDNOTIFY        m_pDSNotify;
	//HINSTANCE                  g_hInst              = NULL;
	GUID                       m_guidCaptureDevice;
	//BOOL                       g_bRecording;
	WAVEFORMATEX               m_wfxInput;
	DSBPOSITIONNOTIFY          m_aPosNotify[ RECORDER_BLOCK_NB + 1 ];
	DWORD					   m_dwNotifySize;
	HANDLE                     m_hNotificationEvent; 
	//BOOL                       g_abInputFormatSupported[16];
	DWORD                      m_dwCaptureBufferSize;
	DWORD                      m_dwNextCaptureOffset;

	SpeexConverter* _converter;
	tBool _is_shutdown_requested;
	FileLog* _filelog;
};