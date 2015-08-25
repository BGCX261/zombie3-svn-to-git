#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "config.hxx"
#include "secudpneter.hxx"
#include "priv/cacherdb.hxx"
#include "priv/cthandler.hxx"

CTHandler::CTHandler(tUInt32 listen_port,FileLog* filelog)
:	ConnHandler(filelog),
	_secudpneter(NULL),
	_cacher(NULL),
	_listen_port(listen_port)
{
}
CTHandler::~CTHandler()
{
}
tBool CTHandler::RecvBridgeMsg(void)
{
	tBridgeMsg* imsg;

	if (!_secudpneter->Recv(&imsg))
		return FALSE;

	SaveIMsg(imsg);

	return TRUE;
}
tBool CTHandler::SendBridgeMsg(void)
{
	if (!_omsg)
	{
		if (!(_omsg = _omsg_que.Pop()))
			return FALSE;
	}

	_secudpneter->SendTo(_omsg,_omsg->rhost,_omsg->rport);

	/**Dont' release the omsg here, leave it to the SecUdpNeter.*/
	_omsg = NULL;

	return TRUE;
}
void CTHandler::PreRunning()
{
	_secudpneter = new SecUdpNeter(_listen_port,10000,_filelog);

	_secudpneter->Start();
}
void CTHandler::AfterRunning()
{
	LOGDEBUG("CTHandler::AfterRunning:Begin.");

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
		LOGWARNING("CTHandler::AfterRunning:SecUdpNeter won't shutdown,sorry but we'll not join any longer,forcing shutting it down.");
		delete _secudpneter;
		_secudpneter = NULL;
	}

	LOGDEBUG("CTHandler::AfterRunning:End.");
}
tBool CTHandler::ProcessIMsg(void)
{
	if (!_imsg)
	{
		if (!(_imsg = _imsg_que.Pop()))
			return FALSE;
	}

	switch (_imsg->tom)
	{
	case E_WAS_MSG_LOGIN_REQ:
		ProcessLoginReq(_imsg);
		break;
	case E_WAS_MSG_LOGOFF_REQ:
		ProcessLogoffReq(_imsg);
		break;
	case E_WAS_MSG_ENDPOINTS_REGISTER_REQ:
		ProcessRegisterUdpEndpoints(_imsg);
		break;
	case E_WAS_MSG_VOICE_CHAT_REQ:
	case E_WAS_MSG_VOICE_GROUP_CHAT_REQ:
	case E_WAS_MSG_VIDEO_CHAT_REQ:
		ProcessChatReq(_imsg);
		break;
	case E_WAS_MSG_TRANSFER_REQ:
		break;
	case E_WAS_MSG_VOICE_CHAT_CNF:
	case E_WAS_MSG_VOICE_GROUP_CHAT_CNF:
	case E_WAS_MSG_VIDEO_CHAT_CNF:
		ProcessChatCnf(_imsg);
		break;
	case E_WAS_MSG_TRANSFER_CNF:
		break;
	case E_WAS_MSG_NOTIFY_ADD:
	case E_WAS_MSG_NOTIFY_DEL:
	case E_WAS_MSG_NOTIFY_LEAVE:
	case E_WAS_MSG_NOTIFY_QUIT:
	case E_WAS_MSG_NOTIFY_CANCEL:
	case E_WAS_MSG_COMMON_CNF:
	case E_WAS_MSG_INSTANTMSG:
	default:
		LOGDEBUG("CTHandler::ProcessIMsg:Forward the imsg #"<<_imsg->tid<<" of tom "<<_imsg->tom<<" from "<<_imsg->rhost<<".");
		ForwardMsg(_imsg);
		break;
	}

	_imsg = NULL;

	return TRUE;
}
tBool CTHandler::DoOTask(void)
{
	return FALSE;
}
void CTHandler::ProcessLoginReq(tBridgeMsg* imsg)
{
	tByte* msg;
	tUInt32 len;
	tUInt32 rspc;
	tBridgeMsg* bmsg;

	tUInt32 userid=0;
	tUInt32 iport=0;
	tChar* id_str;
	tChar* username;
	tChar* internal_ip;

	LOGINFO("CTHandler::ProcessLoginReq:A new user login...");

	tUInt32 mpi = GetAMpi();

	/**
	 * 1)authroze it
	 * 2)register it
	 * 3)confirm it
	 */
	id_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	username = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);
	internal_ip = GetDataFromMsg(imsg,E_WAS_TAG_ID_IHOT);

	CacherDB::GetInstance()->LoginUser(atoi(id_str),username,imsg->rhost,imsg->rport,internal_ip);

	msg = was_write_msg(&len, mpi, E_WAS_MSG_LOGIN_CNF,imsg->tid, X_WAS_RSPC_OK,
						E_WAS_TAG_ID_MYID,1,"1",
						E_WAS_TAG_ID_NONE);
	assert(msg);

	bmsg = CreateBridgeMsg();

	bmsg->rhost = imsg->rhost;
	bmsg->rport = imsg->rport;
	bmsg->lom = len;
	bmsg->mpi = mpi;
	bmsg->tid = imsg->tid;
	bmsg->rspc= X_WAS_RSPC_OK;
	bmsg->msg = msg;
	bmsg->tom = E_WAS_MSG_LOGIN_CNF;
	bmsg->isReceived = FALSE;

	ReleaseBridgeMsg(&imsg);

	SaveOMsg(bmsg);

	LOGDEBUG("CTHandler::ProcessLoginReq:User "<<username<<" #"<<id_str<<" logined.");

	ReleaseDataFromMsg(id_str);
	ReleaseDataFromMsg(username);
	ReleaseDataFromMsg(internal_ip);
}
void CTHandler::ProcessLogoffReq(tBridgeMsg* imsg)
{
	tByte* msg;
	tUInt32 len;
	tUInt32 rspc;
	tBridgeMsg* bmsg;

	tUInt32 userid=0;

	tUInt32 mpi = GetAMpi();

	/**
	 * 1)mark it offline
	 * 2)confirm it
	 */
	tChar* id_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);

	LOGINFO("CTHandler::ProcessLogoffReq:User #"<<id_str<<" logoff...");

	CacherDB::GetInstance()->LogoffUser(atoi(id_str));

	msg = was_write_msg(&len, mpi, E_WAS_MSG_LOGOFF_CNF,imsg->tid, X_WAS_RSPC_OK,
						E_WAS_TAG_ID_NONE);
	assert(msg);

	bmsg = CreateBridgeMsg();

	bmsg->rhost = imsg->rhost;
	bmsg->rport = imsg->rport;
	bmsg->lom = len;
	bmsg->mpi = mpi;
	bmsg->tid = imsg->tid;
	bmsg->rspc= X_WAS_RSPC_OK;
	bmsg->msg = msg;
	bmsg->tom = E_WAS_MSG_LOGOFF_CNF;
	bmsg->isReceived = FALSE;

	ReleaseBridgeMsg(&imsg);

	SaveOMsg(bmsg);

	ReleaseDataFromMsg(id_str);
}
void CTHandler::ProcessRegisterUdpEndpoints(tBridgeMsg* imsg)
{
	tUInt32 userid=0;
	tUInt32 iport=0;
	tBridgeMsg* bmsg;
	tByte* msg;
	tUInt32 len;

	tUInt32 mpi = GetAMpi();

	tUInt32 rspc = X_WAS_RSPC_NONE;

	tChar* userid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	tChar* iport_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_IPOT);

	LOGDEBUG("CTHandler::ProcessRegisterUdpEndpoints:User #"<<userid_str<<" register udp endpoints...");

	if (userid_str)			userid=atoi(userid_str);
	if (iport_str)	iport = atoi(iport_str);

	if (!userid || !(imsg->rport) || !iport)
	{
		LOGWARNING("CTHandler::ProcessRegisterUdpEndpoints:Failed to extract enough info from the imsg.");
		rspc = X_WAS_RSPC_BAD_REQUEST;
	}
	else
	{
		LOGINFO("CTHandler::ProcessRegisterUdpEndpoints:Id="<<userid<<",pport="<<imsg->rport<<",iport="<<iport<<".");

		if (CacherDB::GetInstance()->SetPTUdpEndpoint(userid,imsg->rport,iport))
		{
			rspc = X_WAS_RSPC_OK;
		}
		else
		{
			rspc = X_WAS_RSPC_MEMBER_UNAVAILABLE;
		}
	}

	ReleaseDataFromMsg(userid_str);
	ReleaseDataFromMsg(iport_str);


	msg = was_write_msg(&len, mpi, E_WAS_MSG_ENDPOINTS_REGISTER_CNF,imsg->tid, rspc, 
						E_WAS_TAG_ID_MYID,1,"1",
							E_WAS_TAG_ID_NONE);
	
	if (!msg)
	{
		LOGWARNING("CTHandler::ProcessRegisterUdpEndpoints:Error happened while was_write_msg(...).");
		return;
	}

	bmsg = CreateBridgeMsg();

	bmsg->tid = imsg->tid;
	bmsg->rspc = rspc;
	bmsg->msg = msg;
	bmsg->tom = E_WAS_MSG_ENDPOINTS_REGISTER_CNF;
	bmsg->mpi = mpi;
	bmsg->lom = len;

	bmsg->rhost = imsg->rhost;
	bmsg->rport = imsg->rport;

	bmsg->isReceived = FALSE;

	ReleaseBridgeMsg(&imsg);

	SaveOMsg(bmsg);

	LOGDEBUG("CTHandler::ProcessRegisterUdpEndpoints:User #"<<userid<<" regstering end.");
}
void CTHandler::ProcessChatReq(tBridgeMsg* imsg)
{
	tUInt32 rspc=X_WAS_RSPC_NONE;

	tUInt32 requesterid=0;
	tUInt32 requesteeid=0;
	tChar* requesterid_str;
	tChar* requesteeid_str;
	tChar* requestername;
	tChar* requesteename;

	tUserInfo const * requesterinfo;
	tUserInfo const * requesteeinfo;

	requesterid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	requesteeid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MBID);
	requestername = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);
	requesteename = GetDataFromMsg(imsg,E_WAS_TAG_ID_MBNM);

	LOGDEBUG("CTHandler::ProcessChatReq:"<<requestername<<" #"<< requesterid_str<<" inviting "<<requesteename<<" #"<<requesteeid_str<<".");

	if (requesterid_str)	requesterid = atoi(requesterid_str);
	if (requesteeid_str)	requesteeid = atoi(requesteeid_str);

	if (!requesterid || !requesteeid)
	{
		LOGWARNING("CTHandler::ProcessChatReq:Failed to extract enough from imsg.");

		rspc = X_WAS_RSPC_BAD_REQUEST;
	}
	else if (!(requesterinfo=CacherDB::GetInstance()->GetUserInfo(requesterid)))
	{
		LOGWARNING("CTHandler::ProcessChatReq:Failed to find the info of requester #"<<requesterid<<",logical error.");

		rspc = X_WAS_RSPC_INTERNAL_SERVER_ERROR;
	}
	else if (!(requesteeinfo=CacherDB::GetInstance()->GetUserInfo(requesteeid)) || requesteeinfo->user_state==USER_STATE_UNDEFINED || requesteeinfo->user_state==USER_STATE_OFFLINE)
	{
		LOGWARNING("CTHandler::ProcessChatReq:The requestee #"<<requesteeid<<" is not available.");

		rspc = X_WAS_RSPC_MEMBER_UNAVAILABLE;
	}

	if (rspc!=X_WAS_RSPC_NONE)
	{
		ReleaseBridgeMsg(&imsg);
	}
	else
	{
		tBridgeMsg* bmsg = CreateBridgeMsg();
		tByte* msg;
		tUInt32 len;

		/** 
		 * append requester's endpoints info to the requestee.both voice and video are appended.
		 */
		tChar pport_str[10]={0};
		tChar iport_str[10]={0};
		tChar pport2_str[10]={0};
		tChar iport2_str[10]={0};
		sprintf_s(pport_str,10,"%d",requesterinfo->public_pt_udp_port);
		sprintf_s(iport_str,10,"%d",requesterinfo->internal_pt_udp_port);
		sprintf_s(pport2_str,10,"%d",requesterinfo->public_pt_udp_port2);
		sprintf_s(iport2_str,10,"%d",requesterinfo->internal_pt_udp_port2);

		msg = was_write_msg(&len, imsg->mpi,imsg->tom,imsg->tid,imsg->rspc, 
			E_WAS_TAG_ID_MYID, strlen(requesterid_str), requesterid_str, 
			E_WAS_TAG_ID_MYNM, strlen(requestername), requestername, 
			E_WAS_TAG_ID_MBID, strlen(requesteeid_str), requesteeid_str, 
			E_WAS_TAG_ID_MBNM, strlen(requesteename), requesteename, 
			E_WAS_TAG_ID_PHOT, requesterinfo->public_ip.length(), requesterinfo->public_ip.c_str(), 
			E_WAS_TAG_ID_PPOT, strlen(pport_str), pport_str, 
			E_WAS_TAG_ID_IHOT, requesterinfo->internal_ip.length(), requesterinfo->internal_ip.c_str(),
			E_WAS_TAG_ID_IPOT, strlen(iport_str), iport_str, 
			E_WAS_TAG_ID_PP2T, strlen(pport2_str), pport2_str, 
			E_WAS_TAG_ID_IP2T, strlen(iport2_str), iport2_str, 
			E_WAS_TAG_ID_NONE);

		imsg->lom = len;

		imsg->rhost = requesteeinfo->public_ip;
		imsg->rport = requesteeinfo->public_st_udp_port;

		*bmsg = *imsg;
		bmsg->msg = msg;
		bmsg->isReceived = FALSE;

		ReleaseBridgeMsg(&imsg);
		SaveOMsg(bmsg);

		LOGDEBUG("CTHandler::ProcessChatReq:Fording the request to destination.");
	}

	ReleaseDataFromMsg(requesterid_str);
	ReleaseDataFromMsg(requesteeid_str);
	ReleaseDataFromMsg(requestername);
	ReleaseDataFromMsg(requesteename);
}
void CTHandler::ProcessChatCnf(tBridgeMsg* imsg)
{
	tUInt32 rspc=X_WAS_RSPC_NONE;

	tUInt32 requesterid=0;
	tUInt32 requesteeid=0;

	tChar* requesterid_str;
	tChar* requesteeid_str;
	tChar* adminid_str;

	tUserInfo const* requesterinfo;
	tUserInfo const* requesteeinfo;

	requesteeid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	requesterid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MBID);
	adminid_str		= GetDataFromMsg(imsg,E_WAS_TAG_ID_AMID);
	if (requesteeid_str)	requesteeid = atoi(requesteeid_str);
	if (requesterid_str)	requesterid = atoi(requesterid_str);

	LOGDEBUG("STConn::ProcessChatCnf:Confirmation from requestee #"<<requesteeid_str<<".");

	if (!requesteeid || !requesterid)
	{
		LOGWARNING("CTHandler::ProcessChatReq:Failed to extract enough from imsg,logical error.");

		rspc = X_WAS_RSPC_BAD_REQUEST;
	}
	else if (!(requesteeinfo=CacherDB::GetInstance()->GetUserInfo(requesteeid)))
	{
		LOGWARNING("CTHandler::ProcessChatReq:Failed to find the requestee info,logical error.");

		rspc = X_WAS_RSPC_INTERNAL_SERVER_ERROR;
	}
	else if (!(requesterinfo=CacherDB::GetInstance()->GetUserInfo(requesterid)) || requesterinfo->user_state==USER_STATE_UNDEFINED || requesterinfo->user_state==USER_STATE_OFFLINE)
	{
		LOGWARNING("CTHandler::ProcessChatReq:The requester #"<<requesterid_str<<" is not available,logical error.");

		rspc = X_WAS_RSPC_MEMBER_UNAVAILABLE;
	}

	if (rspc!=X_WAS_RSPC_NONE)
	{
		LOGWARNING("CTHandler::ProcessChatReq:Abandon the confirm msg.");
		return ;
	}

	if (imsg->rspc!=X_WAS_RSPC_OK)
	{
		imsg->rhost = requesterinfo->public_ip;
		imsg->rport = requesterinfo->public_st_udp_port;
		SaveOMsg(imsg);
	}
	else
	{
		tBridgeMsg* bmsg = CreateBridgeMsg();
		tByte* msg;
		tUInt32 len;

		/** append requestee's endpoints info to the requester.*/
		tChar pport_str[10]={0};
		tChar iport_str[10]={0};
		tChar pport2_str[10]={0};
		tChar iport2_str[10]={0};
		sprintf_s(pport_str,10,"%d",requesteeinfo->public_pt_udp_port);
		sprintf_s(iport_str,10,"%d",requesteeinfo->internal_pt_udp_port);
		sprintf_s(pport2_str,10,"%d",requesterinfo->public_pt_udp_port2);
		sprintf_s(iport2_str,10,"%d",requesterinfo->internal_pt_udp_port2);

		msg = was_write_msg(&len, imsg->mpi,imsg->tom,imsg->tid,imsg->rspc, 
			E_WAS_TAG_ID_MYID, strlen(requesteeid_str), requesteeid_str,
			E_WAS_TAG_ID_MYNM, requesteeinfo->name.length(), requesteeinfo->name.c_str(), 
			E_WAS_TAG_ID_MBID, strlen(requesterid_str), requesterid_str, 
			E_WAS_TAG_ID_AMID, strlen(adminid_str), adminid_str, 
			E_WAS_TAG_ID_PHOT, requesteeinfo->public_ip.length(), requesteeinfo->public_ip.c_str(), 
			E_WAS_TAG_ID_PPOT, strlen(pport_str), pport_str, 
			E_WAS_TAG_ID_IHOT, requesteeinfo->internal_ip.length(), requesteeinfo->internal_ip.c_str(),
			E_WAS_TAG_ID_IPOT, strlen(iport_str), iport_str, 
			E_WAS_TAG_ID_PP2T, strlen(pport2_str), pport2_str, 
			E_WAS_TAG_ID_IP2T, strlen(iport2_str), iport2_str, 
			E_WAS_TAG_ID_NONE);

		*bmsg = *imsg;
		bmsg->lom = len;
		bmsg->msg = msg;
		bmsg->isReceived = FALSE;

		bmsg->rhost = requesterinfo->public_ip;
		bmsg->rport = requesterinfo->public_st_udp_port;

		ReleaseBridgeMsg(&imsg);
		SaveOMsg(bmsg);
	}
	
	LOGDEBUG("STConn::ProcessChatCnf:Forwarding Confirmation msg to requester #"<<requesterid_str<<".");

	ReleaseDataFromMsg(requesteeid_str);
	ReleaseDataFromMsg(requesterid_str);
}
void CTHandler::ForwardMsg(tBridgeMsg* imsg)
{
	tBridgeMsg* bmsg;
	tByte* msg;
	tUInt32 len;
	tUInt32 rspc=X_WAS_RSPC_NONE;

	tUInt32 requesterid=0;
	tUInt32 requesteeid=0;

	tChar* requesterid_str;
	tChar* requesteeid_str;

	//tUserInfo const* requesterinfo;
	tUserInfo const* requesteeinfo;

	requesterid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	requesteeid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MBID);
	if (requesteeid_str)	requesteeid = atoi(requesteeid_str);
	if (requesterid_str)	requesterid = atoi(requesterid_str);

	LOGDEBUG("CTHandler::ForwardMsg:From #"<<requesterid_str<<" to #"<<requesteeid_str<<"...");

	if (!requesteeid || !requesterid)
	{
		LOGERROR("CTHandler::ForwardMsg:Failed to extract enough from imsg,logical error.");

		ReleaseBridgeMsg(&imsg);
	}
	else if (!(requesteeinfo=CacherDB::GetInstance()->GetUserInfo(requesteeid)) || 
				requesteeinfo->user_state==USER_STATE_UNDEFINED || 
				requesteeinfo->user_state==USER_STATE_OFFLINE)
	{
		LOGWARNING("CTHandler::ForwardMsg:The requestee #"<<requesteeid<<" is not available,store the msg on CentralS.");

		ReleaseBridgeMsg(&imsg);
	}
	else
	{
		LOGDEBUG("CTHandler::ForwardMsg:Forwarding instant msg to requestee #"<<requesteeid<<".");

		imsg->rhost = requesteeinfo->public_ip;
		imsg->rport = requesteeinfo->public_st_udp_port;

		SaveOMsg(imsg);
	}

	ReleaseDataFromMsg(requesterid_str);
	ReleaseDataFromMsg(requesteeid_str);
}