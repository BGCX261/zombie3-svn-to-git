#ifndef __CHATTER_HXX__
#define __CHATTER_HXX__

#include <list>
#include <map>
#include <string>
#include <iostream>
//#include "mydll.h"
#include "mytypes.h"
#include "mythread.hxx"

using namespace std;

#ifdef WIN32

#ifdef CHATTER_EXPORTS

#ifdef EXPORT_CHATTER
#undef EXPORT_CHATTER
#endif

#define EXPORT_CHATTER __declspec(dllexport)
#else
// the exe imports
#define EXPORT_CHATTER __declspec(dllimport)
#endif

#pragma comment(lib,"Msacm32")
#pragma comment(lib,"libspeex")
#pragma comment(lib,"libspeexdsp")

#pragma comment(lib,"mythread")
#pragma comment(lib,"myfile")
#pragma comment(lib,"mynet")
#pragma comment(lib,"myudpnet")
#pragma comment(lib,"message")
#pragma comment(lib,"bridgenet")
#pragma comment(lib,"axaudio")

#else /* WIN32 */
#define EXPORT_CHATTER
#endif /* WIN32 */

//////////////////////////////////////////////TYPEDEFS//////////////////////////
typedef struct tagBridgeMsg tBridgeMsg;
typedef void (*tUpdateFunc)(unsigned long statusid);

typedef enum
{
	TASK_TYPE_UNDEFINED,
	TASK_TYPE_LOGIN,
	TASK_TYPE_INSTANT_MSG_OUT,
	TASK_TYPE_JOIN_VOICE_CHAT_OUT,
	TASK_TYPE_ADD_VOICE_GROUP_CHAT_OUT,
	TASK_TYPE_INVITE_VEDIO_CHAT_OUT,
	TASK_TYPE_TRANSFER_FILE_OUT,
	TASK_TYPE_INSTANT_MSG,
	TASK_TYPE_JOIN_VOICE_CHAT,
	TASK_TYPE_ADD_VOICE_GROUP_CHAT,	//request the member to join in exsiting group chatting.
	TASK_TYPE_INVITE_VEDIO_CHAT,
	TASK_TYPE_TRANSFER_FILE,
	TASK_TYPE_NUM
}tTaskType;
typedef enum
{
	TASK_STATE_UNDEFINED,
	TASK_STATE_ERROR_HAPPENED,
	TASK_STATE_REQUEST_CREATED,
	TASK_STATE_REQUEST_SENT,
	TASK_STATE_REQUEST_REQUESTED,
	TASK_STATE_REQUEST_CONFIRMED,
	TASK_STATE_REQUEST_REJECTED,
	TASK_STATE_REQUEST_TIMEOUTED,
	TASK_STATE_REQUEST_IGNORED_FOR_BUSY,
	TASK_STATE_PEER_ERROR,
	TASK_STATE_MY_ERROR,
	TASK_STATE_ON_GOING,
	TASK_STATE_CANCELLED,
	TASK_STATE_TERMINATED,
	TASK_STATE_FINISHED,
	TASK_STATE_NUM
}tTState;
typedef struct EXPORT_CHATTER tagTStatus
{
	tUInt32		tstatusid;
	tUInt32		userid;
	string		username;
	tUInt32		tid;
	tTaskType	tasktype;
	tTState		tstate;
	tUInt32		finished;
	tUInt32		total;
	void*		arg1;

	string msgtext();
	string tittle();
	string notification();
	bool isin();
	float progress();
}tTStatus;
typedef list<tTStatus*>	tTStatusList;
typedef tTStatusList::iterator	tTStatusListIter;
typedef tTStatusList::const_iterator	tTStatusListConstIter;

class TStatusMgr;
class GroupMgr;
class STConn;
class WFAudioRecorder;
class UdpSocket;
class MVHandler;
class RecordHandler;
class FileLog;

class EXPORT_CHATTER Chatter	:	public Thread
{
public:
////////////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////////////
	typedef enum
	{
		CHATTER_STATE_UNKNOWN,
		CHATTER_STATE_STARTINGUP,
		CHATTER_STATE_RUNNING,
		CHATTER_STATE_SHUTTINGDOWN,
		CHATTER_STATE_FINISHED
	}tChatterState;

	/**
	 * @brief	:	Constructor
	 * @param update_func	:	Callback when state of the chatter changed. 
	 */
	Chatter(tUpdateFunc update_func,tUInt32 my_id,char* my_name);

	//\brief	destructor
	virtual ~Chatter();

	/**
	 * @brief	: send a instant message to peer user.
	 * @param userid	:
	 * @param instantmsg	:
	 * @return	: the task id or failed with 0
	 */
	tUInt32 SendInstantMsg(tUInt32 userid,char* username,char* instantmsg);

	/**
	 * @brief	: request a member to join the group chat
	 * @param userid	: specifying the member to request
	 * @param centrals_host	:
	 * @param centrals_port	:
	 * @return	: the outgoing task id by which to get the task status,or failed with 0
	 */
	tUInt32 InvitePeerVoiceChat(tUInt32 userid,char* username);
	tUInt32 AddVoiceGroupChat(tUInt32 userid,char* username);
	tUInt32 InvitePeerVideoChat(tUInt32 userid,char* username);
	void NotifyAddNewMember(tUInt32 userid,string username,string host,tUInt32 port);

	/**
	 * @brief	: transfer file file_path_and_name to userid
	 * @param userid	:
	 * @param file_path_and_name	:
	 * @return	: the task id or failed with 0
	 */
	tUInt32 Transfer(tUInt32 userid,char* username,char* file_path_and_name);

	/**
	 * @brief	: Query the status of the specific task,if tid==0, get all task status of the otconn to the remote_host.
	 * @param hconn	: the remote host the task targets at
	 * @param tid			: the task id within the tconn to the remote_host
	 * @return	: the task status or null if there is no such a status registered
	 */
	tTStatus* QueryTStatus(tUInt32 tstatusid);

	void AcceptRequest(tUInt32 tstatusid,char* additional);
	void RejectRequest(tUInt32 tstatusid);
	void CancelTask(tUInt32 tstatusid);

	void Shutdown(void);

	char const * GetMyName(void) const;
	tUInt32 GetMyId(void) const;

	tChatterState ChatterState(void) const;

	tBool EnsureVoiceOpen(void);
	void CloseVoice(void);

	tBool AmIAdmin(void) const;
	void SetMeAdmin(void);
	void ResetMeAdmin(void);

	tBool SetUserAdmin(tUInt32 userid);

	tBool CanIInviteVoice(string& reasonifno);
	tBool CanIInviteVideo(string& reasonifno);

protected:
	//
	friend class STConn;
	void AddMember(tUInt32 userid,string username,string public_ip,tUInt32 public_udp_port,string internal_ip,tUInt32 internal_udp_port);
	void DeleteMember(tUInt32 userid);
	tBool AmIInChatting(void) const;
	void SetChatBusy(tBool val);

	virtual void Execute(Thread::Arg arg);

private:
////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////
	void SetChatterState(tChatterState chatterstate);

	GroupMgr* _groupmgr;
	TStatusMgr* _tstatusmgr;
	STConn* _stconn;
	WFAudioRecorder* _recorder;
	UdpSocket* _vudpsocket;
	MVHandler* _mvhandler;
	RecordHandler* _recordhandler;

	tUpdateFunc _update_func;

	tChatterState _chatter_state;

	string	_myname;
	tUInt32	_myid;
	tBool _is_building_session;

	tBool _is_shutdown_requested;
	FileLog* _filelog;
};

#endif //__CHATTER_HXX__