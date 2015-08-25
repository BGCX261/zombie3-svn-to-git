#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <Dsound.h>

#include "mytypes.h"
#include "mythread.hxx"

#include "priv/dsutil.h"

//class DSPlayer;
//class DSRecorder;
class SpeexConverter;
//class ACMConverter;
struct SpeexJitter;
class FileLog;

using namespace std;

class DDuplexAudio	:	public Thread
{
public:
////////////////////////////////////////////////////////////////////////
// typedef
////////////////////////////////////////////////////////////////////////
	typedef map<string,SpeexJitter*> tFromJitterMap;
	typedef tFromJitterMap::iterator tFromJitterMapIter;
	typedef tFromJitterMap::const_iterator tFromJitterMapConstIter;
	typedef pair<string,SpeexJitter*> tFromJitterPair;

	DDuplexAudio(void* hWnd,FileLog* filelog);

	~DDuplexAudio();

	void Shutdown();

	/**
	 * @brief:Save the incomming voice data and then write merged datas from jitters into palyer
	 */
	void RegisterFrom(string host,tUInt32 port);
	void UnregisterFrom(string host,tUInt32 port);
	tUInt32 GetBlockSize(void) const;

	tUInt32 WriteSound(string host,tUInt32 port, tByte* data, tUInt32 size);
	tUInt32 ReadSound(tByte* buf, tUInt32 bufsize);

	void SetOutVolume(tUInt32 val);
	tUInt32 GetOutVolume(void);
	void SetInVolume(tUInt32 val);
	tUInt32 GetInVolume(void);

	string GetWarning(void);

protected:
	virtual void Execute(Thread::Arg arg);
private:
	DDuplexAudio(const DDuplexAudio& model);

	tBool Init();
	tBool Uninit();

	void ResetPlay();
	void ResetCapture();
	HRESULT StartPlay();
	HRESULT StartCapture();
	void StopPlay();
	void StopCapture();

	HRESULT HandleRenderNotification(void);
	HRESULT HandleCaptureNotification(void);
	HRESULT RecordCapturedData();
	HRESULT FillBufferWithSound(BOOL bResetAndFillAll);
	HRESULT RestoreBuffer( BOOL* pbWasRestored );

	void GetMergedFramFromJitters(tByte* buf,tUInt32 bufsize);

	static void MixVoice(tInt16* voice1,const tInt16* voice2,tUInt32 len);

	HWND m_hWnd;			//handle to application window.
	LPDIRECTSOUNDFULLDUPLEX		m_pDSFD;
	LPDIRECTSOUNDCAPTUREBUFFER8	m_pDSCB;
	LPDIRECTSOUNDBUFFER8		m_pDSB;

	HANDLE _events[2];	// render[0] and capture[1] event.
	//DSBPOSITIONNOTIFY*  aPosNotify0;
	//DSBPOSITIONNOTIFY*  aPosNotify1;
	//LPDIRECTSOUNDNOTIFY pDSNotify0;
	//LPDIRECTSOUNDNOTIFY pDSNotify1;

	DWORD m_dwNextCaptureOffset;
	DWORD m_dwCaptureBufferSize;
	DWORD m_dwNextWriteOffset;
	DWORD m_dwWriteBufferSize;

	DWORD m_dwNotifySize;

	DataBuffer* m_pBufferPlayer;
	DataBuffer* m_pBufferRecorder;

	SpeexConverter* _converter;

	/**
	 * For buffering the incomming voice data
	 */
	 tFromJitterMap _from_jitter_map;

	 string _warning;

	 tBool _is_shutdown_requested;

	 FileLog* _filelog;
};