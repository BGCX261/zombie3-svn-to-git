#ifndef __RECORD_HANDLER_HXX__
#define __RECORD_HANDLER_HXX__

#include "mytypes.h"
#include "mythread.hxx"
#include "priv/groupmgr.hxx"
#include "priv/speexconverter.hxx"

#define USER_SHUTDOWN	(WM_USER + 100)

class AudioRecorder;
class UdpSocket;
class FileLog;

class RecordHandler	:	public Thread
{
public:
	// \Brief Constructor
	RecordHandler(AudioRecorder* _recorder,UdpSocket*udpsocket,GroupMgr* _groupmgr,FileLog* filelog=NULL);

	// \Brief Destructor
	~RecordHandler();

	tInt32	Init(void);
	void	UnInit(void);

	virtual void Execute(Thread::Arg arg);

	void Shutdown();

	
private:
	/**
	 * Hide default copy constructor in case misused.
	 */
	RecordHandler(const RecordHandler &model);

	/**
	 * Send the voice data to a given dest client
	 */
	tUInt32 SendVoice(tByte* voicedata, tUInt32 size, const sockaddr_in* sockaddr_in);

	AudioRecorder*	_audiorecorder;
	UdpSocket*		_udpsocket;
	GroupMgr*		_groupmgr;
	SpeexConverter* _converter;

	BOOL			_is_shutdown_requested;

	FileLog*		_filelog;
};

#endif //__RECORD_HANDLER_HXX__