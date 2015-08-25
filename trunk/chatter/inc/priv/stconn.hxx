// Connection to Central Server
 

#ifndef __STCONN_HXX__
#define __STCONN_HXX__

//#include "mydll.h"
#include "mytypes.h"
#include "priv/tconn.hxx"

//class GroupMgr;
class Chatter;
class TStatusMgr;
class SecUdpNeter;
class FileLog;

class STConn	:	public TConn
{
public:
	
	/**
	 * @brief	:	Constructor
	 */
	STConn(string remote_host,tUInt32 remote_port,Chatter* chatter,TStatusMgr* tstatusmgr,FileLog* filelog = NULL);

	/**
	 * @brief	:	Destructor
	 */
	virtual ~STConn();

	tUInt32 SendInstantMsg(tUInt32 userid,string username,tUInt32 myid,string myname,string instantmsg);

	tUInt32 Login(tUInt32 myid,string myname);

	/**
	 * @brief	:	Invite the user to join in the talk
	 *	1) create the request message
	 *	2) save the message into the out msg que for sending out.
	 *	3) register the task to track the state
	 * @param userid	:	the member to invite
	 * @return	:	task id by which to query status.
	 */
	tUInt32	JoinVoiceChat(tUInt32 userid,string username,tUInt32 myid,string myname);
	tUInt32 AddVoiceGroupChat(tUInt32 userid,string username,tUInt32 myid,string myname);
	tUInt32	InvitePeerVideoChat(tUInt32 userid,string username,tUInt32 myid,string myname);

	/**
	 * @brief	:	ask present members to add this new member,and also ask this new member to add
	 *				all present members go chatting group via conn to server by calling 
	 *				manager->NotifyAddNewMember(...)
	 */
	void	NotifyAddNewMember(tUInt32 new_id,string new_name,string new_host,tUInt32 new_udp_port);

	tUInt32	TransferRequest(tUInt32 userid,string username,tUInt32 myid,string myname,string file_name_and_path);

	string	PeerLocalAddress(void) const;

	void AcceptRequest(tUInt32 userid,tUInt32 tid,tUInt32 myid,char* additional);
	void RejectRequest(tUInt32 userid,tUInt32 tid,tUInt32 myid);
	void CancelTask(tUInt32 userid,tUInt32 tid,tUInt32 myid);
	void TerminateTask(tUInt32 userid,tUInt32 tid,tUInt32 myid);
	
protected:
	//
	virtual tBool RecvBridgeMsg(void);
	virtual tBool SendBridgeMsg(void);
	virtual void	PreRunning(void);
	virtual void	AfterRunning(void);
	virtual tBool	ProcessIMsg(void);

	void ProcessTerminnateINDMsg(tBridgeMsg* imsg);
	void ProcessCancelINDMsg(tBridgeMsg* imsg);
	void ProcessCNFMsg(tBridgeMsg* imsg);
	void ProcessLoginCNFMsg(tBridgeMsg* imsg);
	void ProcessJoinVoiceChatREQMsg(tBridgeMsg* imsg);
	void ProcessJoinVoiceChatCNFMsg(tBridgeMsg* imsg);
	void ProcessAddVoiceGroupChatREQMsg(tBridgeMsg* imsg);
	void ProcessAddVoiceGroupChatCNFMsg(tBridgeMsg* imsg);
	void ProcessInstantMsg(tBridgeMsg* imsg);

	/**
	 * 1)create the request msg,
	 * 2)save it into the omsg_que.
	 * @return	: the created bridge msg.
	 */
	tBridgeMsg* MakeMyRequest(tUInt32 userid,tUInt32 tid,string username,tUInt32 myid,string myname,E_WAS_MSG_TYPE tom,string data="");
	tBridgeMsg* MakeMyAnswer(tUInt32 userid,tUInt32 tid,tUInt32 myid,tUInt32 rspc,E_WAS_MSG_TYPE tom);
	//void Answer2TransferFileRequest(tUInt32 userid,tUInt32 tid,tUInt32 myid,tBool decision,char*fullpath);

private:
	static E_WAS_MSG_TYPE TaskType2MsgType(tTaskType ttype);
	//
	Chatter* _chatter;
	TStatusMgr* _tstatusmgr;
	SecUdpNeter* _secudpneter;
};
#endif //__STCONN_HXX__