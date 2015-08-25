#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "taskdefs.h"
#include "was_api.h"
#include "bridgenet.hxx"
#include "secudpneter.hxx"
#include "chatter.hxx"
#include "priv/tstatusmgr.hxx"
#include "priv/stconn.hxx"

STConn::STConn(string remote_host,tUInt32 remote_port,Chatter* chatter,TStatusMgr* tstatusmgr,FileLog* filelog)
:	TConn(remote_host,remote_port,filelog),
	_chatter(chatter),
	_tstatusmgr(tstatusmgr),
	_secudpneter(NULL)
{
}
STConn::~STConn()
{
}
tUInt32 STConn::Login(tUInt32 myid,string myname)
{
	LOGDEBUG("STConn::Login:Logining...");

	tUInt32 tid = GetATId();

	/**
	 * Create the request message
	 */
	tByte* msg;
	tUInt32 len;
	string ip	= SecUdpNeter::PeerLocalAddress();

	tChar myid_str[10] = {0};
	sprintf_s(myid_str, "%d", myid);

	msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_LOGIN_REQ,tid, X_WAS_RSPC_NONE,
								E_WAS_TAG_ID_MYID, strlen(myid_str), myid_str,
								E_WAS_TAG_ID_MYNM, myname.length(), myname.c_str(),
								E_WAS_TAG_ID_IHOT, ip.length(), ip.c_str(),
								E_WAS_TAG_ID_NONE);

	if (!msg)
	{
		LOGWARNING("STConn::InviteMember:Error happened while was_write_msg(...).");
		return 0;
	}

	tBridgeMsg* bmsg = CreateBridgeMsg();

	bmsg->lom = len;
	bmsg->mpi = E_WAS_MSG_FIRST_PART;
	bmsg->tid = tid;
	bmsg->rspc= X_WAS_RSPC_NONE;
	bmsg->msg = msg;
	bmsg->tom = E_WAS_MSG_LOGIN_REQ;
	bmsg->isReceived = FALSE;

	/*
	 * register the task
	 */
	_tstatusmgr->RegisterOutTStatus(CENTRALS_USER_ID,_remote_host,tid,TASK_TYPE_LOGIN);

	SaveOMsg(bmsg);
	
	return tid;
}
tUInt32 STConn::SendInstantMsg(tUInt32 userid,string username,tUInt32 myid,string myname,string instantmsg)
{
	LOGDEBUG("STConn::SendInstantMsg:Sending instantmsg to "<<username<<" #"<<userid<<"...");

	tUInt32 tid = GetATId();

	tBridgeMsg* bmsg = MakeMyRequest(userid,tid,username,myid,myname,E_WAS_MSG_INSTANTMSG,instantmsg);

	tChar* pmsg = new tChar[instantmsg.length()+1];
	strcpy(pmsg,instantmsg.data());
	pmsg[instantmsg.length()]=0;

	_tstatusmgr->RegisterOutTStatus(userid,username,tid,TASK_TYPE_INSTANT_MSG_OUT,TASK_STATE_REQUEST_CREATED,reinterpret_cast<void*>(pmsg));

	return bmsg->tid;
}
tUInt32 STConn::JoinVoiceChat(tUInt32 userid,string username,tUInt32 myid,string myname)
{
	LOGDEBUG("STConn::JoinVoiceChat:To "<<username<<" #"<<userid<<"...");

	tUInt32 tid = GetATId();

	tTState tstate = TASK_STATE_REQUEST_CREATED;

	MakeMyRequest(userid,tid,username,myid,myname,TaskType2MsgType(TASK_TYPE_JOIN_VOICE_CHAT_OUT));

	_chatter->SetChatBusy(TRUE);

	_tstatusmgr->RegisterOutTStatus(userid,username,tid,TASK_TYPE_JOIN_VOICE_CHAT_OUT,TASK_STATE_REQUEST_CREATED);
	
	return tid;
}
tUInt32 STConn::AddVoiceGroupChat(tUInt32 userid,string username,tUInt32 myid,string myname)
{
	LOGDEBUG("STConn::AddVoiceGroupChat:To "<<username<<" #"<<userid<<"...");

	tUInt32 tid = GetATId();

	MakeMyRequest(userid,tid,username,myid,myname,TaskType2MsgType(TASK_TYPE_ADD_VOICE_GROUP_CHAT_OUT));

	_tstatusmgr->RegisterOutTStatus(userid,username,tid,TASK_TYPE_ADD_VOICE_GROUP_CHAT_OUT,TASK_STATE_REQUEST_CREATED);
	
	return tid;
}
tUInt32 STConn::InvitePeerVideoChat(tUInt32 userid,string username,tUInt32 myid,string myname)
{
	return 0;
}
void STConn::NotifyAddNewMember(tUInt32 new_id,string new_name,string new_host,tUInt32 new_udp_port)
{
	//loop
}
tUInt32 STConn::TransferRequest(tUInt32 userid,string username,tUInt32 myid,string myname,string file_path_and_name)
{
	tUInt32 tid = GetATId();

	return tid;
}
tBool STConn::RecvBridgeMsg(void)
{
	tBridgeMsg* imsg;

	if (!_secudpneter->Recv(&imsg))
		return FALSE;
	
	SaveIMsg(imsg);

	return TRUE;
}
tBool STConn::SendBridgeMsg(void)
{
	if (_secudpneter->AnyOMsgFailed())
	{
		//update GUI for the failure so the user can know.
	}

	/** 
	 * Check if any message sending failed,and update status for them.do it later.
	 */
	if (!_omsg)
	{
		if (!(_omsg = _omsg_que.Pop()))
			return FALSE;
	}

	_secudpneter->SendTo(_omsg,_remote_host,_remote_port);

	//_tstatusmgr->UpdateTStatus(_omsg->uid,_omsg->tid,TASK_STATE_REQUEST_SENT);

	/** 
	 * As soon as sent,release the omsg.but we don't release for secudpneter,for here
	 * it is just pushing the omsg into the out going que.Leave it to the secudpneter.
	 */
	//ReleaseBridgeMsg(&_omsg);

	_omsg = NULL;

	return TRUE;
}
void STConn::PreRunning(void)
{
	LOGDEBUG("STConn::PreRunning:Begin.");

	_secudpneter = new SecUdpNeter(10000,_filelog);

	_secudpneter->Start();

	LOGDEBUG("STConn::PreRunning:End.");
}
void STConn::AfterRunning(void)
{
	LOGDEBUG("STConn::AfterRunning:Begin.");

	_secudpneter->Shutdown();

	tUInt32 counter=0;
	while (counter<100 && (_secudpneter))
	{
		if (!_secudpneter && _secudpneter->State()==Thread::FINISHED_STATE)
		{
			_secudpneter->Join();
			delete _secudpneter;
			_secudpneter = NULL;
		}

		SleepMs(100);

		counter++;
	}

	if (counter==100)
	{
		LOGWARNING("STConn::AfterRunning:SecUdpNeter won't shutdown,sorry but we'll not join any longer,forcing shutting it down.");
		delete _secudpneter;
		_secudpneter = NULL;
	}

	LOGDEBUG("STConn::AfterRunning:End.");
}
tBool STConn::ProcessIMsg(void)
{
	if (!_imsg)
	{
		if (!(_imsg = _imsg_que.Pop()))
			return FALSE;
	}

	switch (_imsg->tom)
	{
	case E_WAS_MSG_INSTANTMSG:
		ProcessInstantMsg(_imsg);
		break;
	case E_WAS_MSG_JOINVOICECHAT_REQ:
		ProcessJoinVoiceChatREQMsg(_imsg);
		break;
	case E_WAS_MSG_ADDVOICEGROUPCHAT_REQ:
		ProcessAddVoiceGroupChatREQMsg(_imsg);
		break;
	case E_WAS_MSG_INVITEVIDEOCHAT_REQ:
		ProcessJoinVoiceChatREQMsg(_imsg);
		break;
	case E_WAS_MSG_ADDMEMBER_REQ:
		break;
	case E_WAS_MSG_TRANSFER_REQ:
		break;
	case E_WAS_MSG_LOGIN_CNF:
		ProcessLoginCNFMsg(_imsg);
		break;
	case E_WAS_MSG_JOINVOICECHAT_CNF:
	case E_WAS_MSG_ADDVOICEGROUPCHAT_CNF:
	case E_WAS_MSG_INVITEVIDEOCHAT_CNF:
	case E_WAS_MSG_TRANSFER_CNF:
	case E_WAS_MSG_ADDMEMBER_CNF:
	case E_WAS_MSG_COMMON_CNF:
		ProcessCNFMsg(_imsg);
		break;
	case E_WAS_MSG_CANCEL_IND:
		ProcessCancelINDMsg(_imsg);
		break;
	case E_WAS_MSG_TERMINATE_IND:
		ProcessTerminnateINDMsg(_imsg);
		break;
	default:
		LOGERROR("STConn::ProcessIMsg:Unknown CNF msg #"<<_imsg->tid<<" from "<<_imsg->rhost<<"::"<<_imsg->rport<<",abandon it.");
		ReleaseBridgeMsg(&_imsg);
		break;
	}

	/*As it's processed.leave it to the Process... respectively.*/
	_imsg = NULL;

	return TRUE;
}
void STConn::ProcessCancelINDMsg(tBridgeMsg* imsg)
{
	tUInt32 userid=0;
	tChar* userid_str = GetDataFromMsg(_imsg,E_WAS_TAG_ID_MYID);
	
	if (userid_str) userid = atoi(userid_str);

	LOGDEBUG("STConn::ProcessCancelINDMsg:CancelINDMsg tid #"<<imsg->tid<<" from #"<<userid_str<<".");
	
	ReleaseDataFromMsg(userid_str);

	tTStatus* tstatus=NULL;

	if (!userid || !(tstatus=_tstatusmgr->QueryInTStatus(userid,_imsg->tid)))
	{
		ReleaseBridgeMsg(&_imsg);
		return;
	}

	_tstatusmgr->UpdateInTStatus(userid,_imsg->tid,TASK_STATE_CANCELLED);

	ReleaseBridgeMsg(&_imsg);
}
void STConn::ProcessTerminnateINDMsg(tBridgeMsg* imsg)
{
	tUInt32 userid=0;
	tChar* userid_str = GetDataFromMsg(_imsg,E_WAS_TAG_ID_MYID);
	
	if (userid_str) userid = atoi(userid_str);

	LOGDEBUG("STConn::ProcessCancelINDMsg:TerminateINDMsg tid #"<<imsg->tid<<" from #"<<userid_str<<".");
	
	ReleaseDataFromMsg(userid_str);

	tTStatus* tstatus=NULL;

	if (!userid || !(tstatus=_tstatusmgr->QueryOutTStatus(userid,_imsg->tid)))
	{
		ReleaseBridgeMsg(&_imsg);
		return;
	}

	_tstatusmgr->UpdateOutTStatus(userid,_imsg->tid,TASK_STATE_TERMINATED);

	ReleaseBridgeMsg(&_imsg);
}
void STConn::ProcessCNFMsg(tBridgeMsg* imsg)
{
	tUInt32 userid=0;
	tChar* userid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	if (userid_str) userid = atoi(userid_str);
	ReleaseDataFromMsg(userid_str);

	tTStatus* tstatus=NULL;

	if (!userid || !(tstatus=_tstatusmgr->QueryOutTStatus(userid,imsg->tid)))
	{
		ReleaseBridgeMsg(&imsg);
		return;
	}

	switch (tstatus->tasktype)
	{
	case TASK_TYPE_JOIN_VOICE_CHAT:
	case TASK_TYPE_ADD_VOICE_GROUP_CHAT:
		ProcessJoinVoiceChatCNFMsg(imsg);
		break;
	case TASK_TYPE_INVITE_VEDIO_CHAT:
	case TASK_TYPE_TRANSFER_FILE:
	default:
		break;
	}
}
void STConn::ProcessLoginCNFMsg(tBridgeMsg* imsg)
{
	tTState tstate;;

	switch (imsg->rspc)
	{
	case X_WAS_RSPC_OK:
		LOGDEBUG("STConn::ProcessJoinVoiceChatCNFMsg:Positively confirmed.");
		tstate = TASK_STATE_REQUEST_CONFIRMED;
		break;
	case X_WAS_RSPC_CENTRALS_AUTHORIZATION_FAILED:
		LOGWARNING("STConn::ProcessJoinVoiceChatCNFMsg:Authorization failed by CentralS.");
		tstate = TASK_STATE_REQUEST_REJECTED;
		break;
	case X_WAS_RSPC_CENTRALS_FORWARDED:
		LOGDEBUG("STConn::ProcessJoinVoiceChatCNFMsg:The current CentralS is busy,turn to another CentralS.");
		tstate = TASK_STATE_ON_GOING;
		break;
	case X_WAS_RSPC_CENTRALS_BUSY:
		LOGDEBUG("STConn::ProcessJoinVoiceChatCNFMsg:The CentralS system is busy,login failed.");
		tstate = TASK_STATE_REQUEST_REJECTED;
		break;
	case X_WAS_RSPC_BAD_REQUEST:
		LOGWARNING("STConn::ProcessJoinVoiceChatCNFMsg:Logically error,X_WAS_RSPC_BAD_REQUEST returned from CentralS.");
		tstate = TASK_STATE_ERROR_HAPPENED;
		break;
	default:
		LOGWARNING("STConn::ProcessLoginCNFMsg:Unknown rspc from CentralS'"<<imsg->rspc<<"'.");
		tstate = TASK_STATE_ERROR_HAPPENED;
		break;
	}

	if (!_tstatusmgr->UpdateOutTStatus(CENTRALS_USER_ID,imsg->tid,tstate))
		LOGWARNING("STConn::ProcessLoginCNFMsg:Failed to UpdateTStatus for login cnf imsg.");

	ReleaseBridgeMsg(&imsg);
}
void STConn::ProcessJoinVoiceChatREQMsg(tBridgeMsg* imsg)
{
	tUInt32 requesterid;

	tChar* requesterid_str				= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	tChar* requestername				= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);
	tChar* public_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_PHOT);
	tChar* internal_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_IHOT);
	tChar* public_pt_udp_port_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_PPOT);
	tChar* internal_pt_udp_port_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_IPOT);

	if (requesterid_str) requesterid = atoi(requesterid_str);

	LOGDEBUG("STConn::ProcessJoinVoiceChatREQMsg:User '"<<requestername<<" #"<<requesterid<<"' is inviting me to talk.");

	tEndPoints* endpoints = new tEndPoints;
	endpoints->public_ip = public_ip;
	endpoints->public_pt_udp_port = atoi(public_pt_udp_port_str);
	endpoints->internal_ip = internal_ip;
	endpoints->internal_pt_udp_port = atoi(internal_pt_udp_port_str);

	/*
	 * Register the status.
	 */
	tTState tstate = TASK_STATE_REQUEST_REQUESTED;

	if (_chatter->AmIInChatting() && !_chatter->AmIAdmin())
		tstate = TASK_STATE_REQUEST_IGNORED_FOR_BUSY;

	_tstatusmgr->RegisterInTStatus(requesterid,requestername,imsg->tid,TASK_TYPE_JOIN_VOICE_CHAT,tstate,reinterpret_cast<void*>(endpoints));

	ReleaseDataFromMsg(requesterid_str);
	ReleaseDataFromMsg(requestername);
	ReleaseDataFromMsg(public_ip);
	ReleaseDataFromMsg(internal_ip);
	ReleaseDataFromMsg(public_pt_udp_port_str);
	ReleaseDataFromMsg(internal_pt_udp_port_str);
}
void STConn::ProcessJoinVoiceChatCNFMsg(tBridgeMsg* imsg)
{
	tUInt32 userid;
	tChar* userid_str;
	tChar* username;
	tChar* public_ip;
	tChar* internal_ip;
	tUInt32 public_pt_udp_port;
	tChar* public_pt_udp_port_str;
	tUInt32 internal_pt_udp_port;
	tChar* internal_pt_udp_port_str;

	tTState tstate;;

	switch (imsg->rspc)
	{
	case X_WAS_RSPC_OK:
		tstate = TASK_STATE_REQUEST_CONFIRMED;

		userid_str					= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
		username					= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);
		public_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_PHOT);
		internal_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_IHOT);
		public_pt_udp_port_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_PPOT);
		internal_pt_udp_port_str	= GetDataFromMsg(imsg,E_WAS_TAG_ID_IPOT);

		if (userid_str)					userid = atoi(userid_str);
		if (public_pt_udp_port_str)		public_pt_udp_port = atoi(public_pt_udp_port_str);
		if (internal_pt_udp_port_str)	internal_pt_udp_port = atoi(internal_pt_udp_port_str);

		LOGDEBUG("STConn::ProcessJoinVoiceChatCNFMsg:Peer "<<username<<" #"<<userid<<"' accepted my invitation.");

		_chatter->AddMember(userid,username,public_ip,public_pt_udp_port,internal_ip,internal_pt_udp_port);
		_chatter->SetMeAdmin();

		ReleaseDataFromMsg(userid_str);
		ReleaseDataFromMsg(username);
		ReleaseDataFromMsg(public_ip);
		ReleaseDataFromMsg(internal_ip);
		ReleaseDataFromMsg(public_pt_udp_port_str);
		ReleaseDataFromMsg(internal_pt_udp_port_str);
		break;
	case X_WAS_RSPC_OK_BUT:
		tstate = TASK_STATE_REQUEST_CONFIRMED;

		userid_str					= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
		username					= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);
		public_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_PHOT);
		internal_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_IHOT);
		public_pt_udp_port_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_PPOT);
		internal_pt_udp_port_str	= GetDataFromMsg(imsg,E_WAS_TAG_ID_IPOT);

		if (userid_str)					userid = atoi(userid_str);
		if (public_pt_udp_port_str)		public_pt_udp_port = atoi(public_pt_udp_port_str);
		if (internal_pt_udp_port_str)	internal_pt_udp_port = atoi(internal_pt_udp_port_str);

		LOGDEBUG("STConn::ProcessJoinVoiceChatCNFMsg:Peer "<<username<<" #"<<userid<<" already in chatting, accepted my invitation.");

		_chatter->AddMember(userid,username,public_ip,public_pt_udp_port,internal_ip,internal_pt_udp_port);
		_chatter->SetUserAdmin(userid);

		ReleaseDataFromMsg(userid_str);
		ReleaseDataFromMsg(username);
		ReleaseDataFromMsg(public_ip);
		ReleaseDataFromMsg(internal_ip);
		ReleaseDataFromMsg(public_pt_udp_port_str);
		ReleaseDataFromMsg(internal_pt_udp_port_str);
		break;
	case X_WAS_RSPC_MEMBER_REJECTED:
		LOGDEBUG("STConn::ProcessJoinVoiceChatCNFMsg:The peer requestee rejected my invitation to talk.");
		tstate = TASK_STATE_REQUEST_REJECTED;
		break;
	case X_WAS_RSPC_BAD_REQUEST:
		LOGWARNING("STConn::ProcessJoinVoiceChatCNFMsg:X_WAS_RSPC_BAD_REQUEST returned from CentralS.");
		break;
	case X_WAS_RSPC_INTERNAL_SERVER_ERROR:
		LOGWARNING("STConn::ProcessJoinVoiceChatCNFMsg:X_WAS_RSPC_INTERNAL_SERVER_ERROR returned from CentralS.");
		break;
	default:
		LOGWARNING("STConn::ProcessJoinVoiceChatCNFMsg:Unknown rspc returned '"<<imsg->rspc<<"'.");
		tstate = TASK_STATE_ERROR_HAPPENED;
		break;
	}

	if (!_tstatusmgr->UpdateOutTStatus(userid,imsg->tid,tstate))
		LOGWARNING("STConn::ProcessLoginCNFMsg:Failed to UpdateTStatus for invitation cnf imsg.");

	_chatter->SetChatBusy(FALSE);

	ReleaseBridgeMsg(&imsg);
}
void STConn::ProcessAddVoiceGroupChatREQMsg(tBridgeMsg* imsg)
{
	tUInt32 requesterid;

	tChar* requesterid_str				= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	tChar* requestername				= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);
	tChar* public_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_PHOT);
	tChar* internal_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_IHOT);
	tChar* public_pt_udp_port_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_PPOT);
	tChar* internal_pt_udp_port_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_IPOT);

	if (requesterid_str) requesterid = atoi(requesterid_str);

	LOGDEBUG("STConn::ProcessAddVoiceGroupChatREQMsg:User '"<<requestername<<" #"<<requesterid<<"' is inviting me to talk.");

	tEndPoints* endpoints = new tEndPoints;
	endpoints->public_ip = public_ip;
	endpoints->public_pt_udp_port = atoi(public_pt_udp_port_str);
	endpoints->internal_ip = internal_ip;
	endpoints->internal_pt_udp_port = atoi(internal_pt_udp_port_str);

	/*
	 * Register the status.
	 */
	tUInt32 rspc;
	tTState tstate = TASK_STATE_REQUEST_REQUESTED;
	if (_chatter->AmIInChatting())
		tstate = TASK_STATE_REQUEST_IGNORED_FOR_BUSY;

	_tstatusmgr->RegisterInTStatus(requesterid,requestername,imsg->tid,TASK_TYPE_JOIN_VOICE_CHAT,tstate,reinterpret_cast<void*>(endpoints));

	ReleaseDataFromMsg(requesterid_str);
	ReleaseDataFromMsg(requestername);
	ReleaseDataFromMsg(public_ip);
	ReleaseDataFromMsg(internal_ip);
	ReleaseDataFromMsg(public_pt_udp_port_str);
	ReleaseDataFromMsg(internal_pt_udp_port_str);
}
void STConn::ProcessAddVoiceGroupChatCNFMsg(tBridgeMsg* imsg)
{
	tTState tstate;

	tChar* requesterid_str				= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	tChar* requestername				= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);

	if (imsg->rspc==X_WAS_RSPC_OK)
	{
		tstate = TASK_STATE_REQUEST_CONFIRMED;
		tChar* public_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_PHOT);
		tChar* internal_ip					= GetDataFromMsg(imsg,E_WAS_TAG_ID_IHOT);
		tChar* public_pt_udp_port_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_PPOT);
		tChar* internal_pt_udp_port_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_IPOT);

		LOGDEBUG("STConn::ProcessAddVoiceGroupChatCNFMsg:User '"<<requestername<<" #"<<requesterid_str<<"' accepted my invitation.");

		_chatter->AddMember(atoi(requesterid_str),requestername,public_ip,atoi(public_pt_udp_port_str),internal_ip,atoi(internal_pt_udp_port_str));
	}
	else
	{
		tstate = TASK_STATE_REQUEST_REJECTED;
		LOGDEBUG("STConn::ProcessAddVoiceGroupChatCNFMsg:User '"<<requestername<<" #"<<requesterid_str<<"' rejected my invitation.");
	}

}
void STConn::ProcessInstantMsg(tBridgeMsg* imsg)
{
	tUInt32 userid;
	tChar* userid_str;
	tChar* username;
	tChar* instantmsg;

	userid_str	= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	username	= GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);
	instantmsg	= GetDataFromMsg(imsg,E_WAS_TAG_ID_IMSG);
	
	if (userid_str)	userid = atoi(userid_str);

	LOGDEBUG("STConn::ProcessInstantMsg:Instant message from '"<<username<<" #"<<userid<<"' received.");

	/**
	 * Register the status so that UI can check it.
	 */
	_tstatusmgr->RegisterInTStatus(userid,username,imsg->tid,TASK_TYPE_INSTANT_MSG,TASK_STATE_REQUEST_REQUESTED,instantmsg);

	ReleaseDataFromMsg(userid_str);
	ReleaseDataFromMsg(username);
	ReleaseDataFromMsg(instantmsg);

	LOGDEBUG("STConn::ProcessInstantMsg:Instant message done.");
}
void STConn::AcceptRequest(tUInt32 userid,tUInt32 tid,tUInt32 myid,char* additional)
{
	tTStatus* tstatus;
	tEndPoints* endpoints;

	if (!(tstatus=_tstatusmgr->QueryInTStatus(userid,tid)) || !(endpoints = reinterpret_cast<tEndPoints*>(tstatus->arg1)))
		return;

	tUInt32 rspc = X_WAS_RSPC_NONE;
	tTState tstate = TASK_STATE_UNDEFINED;
	
	LOGDEBUG("STConn::AcceptRequest:I accepted the invitation to chat from "<<tstatus->username<<" #"<<tstatus->userid<<"'.");

	switch (tstatus->tasktype)
	{
	case TASK_TYPE_JOIN_VOICE_CHAT:
		if (!_chatter->EnsureVoiceOpen())
			tstate = TASK_STATE_MY_ERROR;
		else
		{
			if (_chatter->AmIInChatting() && !_chatter->AmIAdmin())
				tstate = TASK_STATE_REQUEST_IGNORED_FOR_BUSY;
			else
			{
				if(_chatter->AmIInChatting())
					rspc = X_WAS_RSPC_OK_BUT;
				else 
					rspc = X_WAS_RSPC_OK;

				_chatter->AddMember(tstatus->userid,tstatus->username,endpoints->public_ip,endpoints->public_pt_udp_port,endpoints->internal_ip,endpoints->internal_pt_udp_port);
				if (!_chatter->AmIAdmin()) _chatter->SetUserAdmin(tstatus->userid);

				tstate = TASK_STATE_REQUEST_CONFIRMED;
			}
		}
		break;
	case TASK_TYPE_ADD_VOICE_GROUP_CHAT:
		if (!_chatter->EnsureVoiceOpen())
			tstate = TASK_STATE_MY_ERROR;
		else
		{
			_chatter->AddMember(tstatus->userid,tstatus->username,endpoints->public_ip,endpoints->public_pt_udp_port,endpoints->internal_ip,endpoints->internal_pt_udp_port);
			_chatter->SetUserAdmin(tstatus->userid);

			if (_chatter->AmIInChatting())
				tstate = TASK_STATE_REQUEST_IGNORED_FOR_BUSY;
			else 
			{
				rspc = X_WAS_RSPC_OK;
				tstate = TASK_STATE_REQUEST_CONFIRMED;
			}
		}
		break;		
	case TASK_TYPE_INVITE_VEDIO_CHAT:
	case TASK_TYPE_TRANSFER_FILE:
	default:
		break;
	}

	if (rspc!=X_WAS_RSPC_NONE) MakeMyAnswer(userid,tid,myid,rspc,TaskType2MsgType(tstatus->tasktype));

	if (tstate!=TASK_STATE_UNDEFINED) _tstatusmgr->UpdateInTStatus(userid,tid,tstate);
}
void STConn::RejectRequest(tUInt32 userid,tUInt32 tid,tUInt32 myid)
{
	tTStatus* tstatus;

	if (!(tstatus=_tstatusmgr->QueryInTStatus(userid,tid)))
		return;

	MakeMyAnswer(userid,tid,myid,X_WAS_RSPC_MEMBER_REJECTED,TaskType2MsgType(tstatus->tasktype));

	_tstatusmgr->UpdateInTStatus(userid,tid,TASK_STATE_REQUEST_REJECTED);
}
void STConn::CancelTask(tUInt32 userid,tUInt32 tid,tUInt32 myid)
{
	tTStatus* tstatus;

	if (!(tstatus=_tstatusmgr->QueryOutTStatus(userid,tid)))
		return;

	MakeMyAnswer(userid,tid,myid,X_WAS_RSPC_NONE,E_WAS_MSG_CANCEL_IND);

	_tstatusmgr->UpdateOutTStatus(userid,tid,TASK_STATE_CANCELLED);
}
void STConn::TerminateTask(tUInt32 userid,tUInt32 tid,tUInt32 myid)
{
	tTStatus* tstatus;

	if (!(tstatus=_tstatusmgr->QueryInTStatus(userid,tid)))
		return;

	MakeMyAnswer(userid,tid,myid,X_WAS_RSPC_NONE,E_WAS_MSG_TERMINATE_IND);

	_tstatusmgr->UpdateInTStatus(userid,tid,TASK_STATE_TERMINATED);
}
tBridgeMsg* STConn::MakeMyRequest(tUInt32 userid,tUInt32 tid,string username,tUInt32 myid,string myname,E_WAS_MSG_TYPE tom,string data)
{
	/**
	 * Create the request message
	 */
	tUInt32 len = 0;
	tByte* msg;

	tChar myid_str[10] = {0};
	sprintf_s(myid_str, "%d", myid);
	tChar userid_str[10] = {0};
	sprintf_s(userid_str, "%d", userid);

	if (E_WAS_MSG_INSTANTMSG==tom)
	{
		msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, tom,tid, X_WAS_RSPC_NONE,
								E_WAS_TAG_ID_MYID, strlen(myid_str), myid_str, 
								E_WAS_TAG_ID_MYNM, myname.length(), myname.c_str(),
								E_WAS_TAG_ID_MBID, strlen(userid_str), userid_str,
								E_WAS_TAG_ID_MBNM, username.length(), username.c_str(),
								E_WAS_TAG_ID_IMSG, data.length(), data.c_str(),
								E_WAS_TAG_ID_NONE);
	}
	else
	{
		msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, tom,tid, X_WAS_RSPC_NONE,
								E_WAS_TAG_ID_MYID, strlen(myid_str), myid_str, 
								E_WAS_TAG_ID_MYNM, myname.length(), myname.c_str(),
								E_WAS_TAG_ID_MBID, strlen(userid_str), userid_str,
								E_WAS_TAG_ID_MBNM, username.length(), username.c_str(),
								E_WAS_TAG_ID_NONE);
	}

	

	if (!msg)
	{
		LOGWARNING("STConn::InviteMember:Error happened while was_write_msg(...).");
		return 0;
	}

	tBridgeMsg* bmsg = CreateBridgeMsg();

	bmsg->lom = len;
	bmsg->mpi = E_WAS_MSG_FIRST_PART;
	bmsg->tid = tid;
	bmsg->rspc= X_WAS_RSPC_NONE;
	bmsg->msg = msg;
	bmsg->tom = tom;
	bmsg->isReceived = FALSE;

	SaveOMsg(bmsg);
	
	return bmsg;
}
tBridgeMsg* STConn::MakeMyAnswer(tUInt32 userid,tUInt32 tid,tUInt32 myid,tUInt32 rspc,E_WAS_MSG_TYPE tom)
{
	tByte* msg;
	tUInt32 len;

	tChar myid_str[16]={0};
	tChar userid_str[16]={0};
	sprintf(myid_str,"%ul",myid);
	sprintf(userid_str,"%ul",userid);

	msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, tom,tid, rspc,
							E_WAS_TAG_ID_MBID,strlen(userid_str),userid_str,
							E_WAS_TAG_ID_MYID,strlen(myid_str),myid_str,
							E_WAS_TAG_ID_NONE);
	if (!msg)
	{
		LOGWARNING("STConn::MakeMyAnswer:Error happened while was_write_msg(...).");
		return 0;
	}

	tBridgeMsg* bmsg = CreateBridgeMsg();

	bmsg->lom = len;
	bmsg->mpi = E_WAS_MSG_FIRST_PART;
	bmsg->tid = tid;
	bmsg->rspc= rspc;
	bmsg->msg = msg;
	bmsg->tom = tom;
	bmsg->isReceived = FALSE;

	SaveOMsg(bmsg);

	return bmsg;
}
E_WAS_MSG_TYPE STConn::TaskType2MsgType(tTaskType ttype)
{
	E_WAS_MSG_TYPE tom;
	switch(ttype)
	{
	case TASK_TYPE_JOIN_VOICE_CHAT_OUT:
		tom = E_WAS_MSG_JOINVOICECHAT_REQ;
		break;
	case TASK_TYPE_ADD_VOICE_GROUP_CHAT_OUT:
		tom = E_WAS_MSG_ADDVOICEGROUPCHAT_REQ;
		break;
	case TASK_TYPE_INVITE_VEDIO_CHAT_OUT:
		tom = E_WAS_MSG_INVITEVIDEOCHAT_REQ;
		break;
	case TASK_TYPE_TRANSFER_FILE_OUT:
		tom = E_WAS_MSG_TRANSFER_REQ;
		break;
	case TASK_TYPE_JOIN_VOICE_CHAT:
		tom = E_WAS_MSG_JOINVOICECHAT_CNF;
		break;
	case TASK_TYPE_ADD_VOICE_GROUP_CHAT:
		tom = E_WAS_MSG_ADDVOICEGROUPCHAT_CNF;
		break;
	case TASK_TYPE_INVITE_VEDIO_CHAT:
		tom = E_WAS_MSG_INVITEVIDEOCHAT_CNF;
		break;
	case TASK_TYPE_TRANSFER_FILE:
		tom = E_WAS_MSG_TRANSFER_CNF;
		break;
	case TASK_TYPE_INSTANT_MSG:
		tom = E_WAS_MSG_INSTANTMSG;
		break;
	default:
		tom = E_WAS_MSG_INVALID;
		break;
	}
	return tom;
}