#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "config.hxx"
#include "tcpsocket.hxx"
#include "tcpbridgenet.hxx"
#include "priv/cacherdb.hxx"
#include "priv/cconnmanager.hxx"
#include "priv/cconnhandler.hxx"

CConnHandler::CConnHandler(CConnManager* manager,tBool permanent,FileLog* filelog)
:	ConnHandler(filelog),
	_socket_id_counter(1),
	_id_socket_map(),
	_logined_id_socket_map(),
	_id_bridgenet_map(),
	_manager(manager),
	_cacher(NULL),
	_permanent(permanent)
{
	_bridgenet = new TcpBridgeNet(_filelog);
}
CConnHandler::~CConnHandler()
{
	if (_bridgenet)	delete _bridgenet;
}
tBool CConnHandler::CanContainMore(void) const
{
	return (_id_socket_map.size() < CENTRAL_HANDLER_CCONN_MAX);
}
tUInt32 CConnHandler::SocketNum(void) const
{
	return static_cast<tUInt32>(_logined_id_socket_map.size());
}
void CConnHandler::SaveSocket(TcpSocket* socket)
{
	_id_socket_map.insert(tIdSocketPair(GetASocketId(),socket));
}
tBool CConnHandler::ShallShutdown(void) const
{
	return (_id_socket_map.empty() && !_permanent);
}
tBool CConnHandler::RecvBridgeMsg(void)
{
	tBool ret = FALSE;

	TcpBridgeNet* bridgenet;
	tBridgeMsg* imsg;
	BridgeNet::tBridgeNetStatus status;
	//tBool is_special_bridgenet;

	tIdSocketIter it;
	for (it=_id_socket_map.begin();it!=_id_socket_map.end();)
	{
		if (!it->second->IsReadyForRead())
		{
			it++;
			Yield();
			continue;
		}

		_bridgenet->SetTcpSocket(it->second);

		status = _bridgenet->ReceiveMsg(&imsg);

		switch (status)
		{
		case BridgeNet::eEndOfConnectionReceived:
		case BridgeNet::eNetworkErrorReceived:
			it->second->Disconnect();
			delete it->second;
			it = _logined_id_socket_map.erase(it);
			ret = TRUE;
			break;
		case BridgeNet::eMsgAvailable:
			if (imsg->tom==E_WAS_MSG_LOGIN_REQ)
				SaveIMsg(imsg);
			ret = TRUE;
		case BridgeNet::eMsgNotAvailable:
		default:
			it++;
			break;
		}
	}

	/** received logined member's msg.*/
	for (it=_logined_id_socket_map.begin();it!=_logined_id_socket_map.end();)
	{
		if (!it->second->IsReadyForRead())
		{
			it++;
			Yield();
			continue;
		}

		_bridgenet->SetTcpSocket(it->second);

		status = _bridgenet->ReceiveMsg(&imsg);

		switch (status)
		{
		case BridgeNet::eEndOfConnectionReceived:
		case BridgeNet::eNetworkErrorReceived:
			it->second->Disconnect();
			delete it->second;
			it = _logined_id_socket_map.erase(it);
			ret = TRUE;
			break;
		case BridgeNet::eMsgAvailable:
			if (imsg->tom==E_WAS_MSG_LOGIN_REQ)
				SaveIMsg(imsg);
			ret = TRUE;
		case BridgeNet::eMsgNotAvailable:
		default:
			it++;
			break;
		}
	}

	return ret;
}
tBool CConnHandler::SendBridgeMsg(void)
{
	tBool ret = FALSE;

	TcpSocket* socket;
	tBridgeMsg* omsg;
	tUInt32 memid=0;
	tChar* memid_str;

	tUInt32 omsg_num = _omsg_que.Size();

	for (tUInt32 i=0;i<omsg_num;i++)
	{
		omsg = _omsg_que.Pop();

		/** Get the destinate Id.*/
		memid_str = GetDataFromMsg(omsg,E_WAS_TAG_ID_MBID);
		if (memid_str) memid = atoi(memid_str);

		assert(memid);

		if (!(socket=FindSocket(memid)))
		{
			LOGDEBUG("CConnHandler::SendBridgeMsg:failed to find logined member #"<<memid<<".");
			ReleaseBridgeMsg(omsg);
		}
		else
		{
			if (!socket->IsReadyForWrite())
			{
				/**We won't wait,push it back into the omsg_que.*/
				SaveOMsg(omsg);
				omsg = NULL;
			}
			else
			{
				LOGDEBUG("CConnHandler::SendBridgeMsg:send msg to #"<<memid<<".");

				ret = TRUE;

				_bridgenet->SetTcpSocket(socket);
				
				_bridgenet->SendBridgeMsg(omsg);

				ReleaseBridgeMsg(omsg);
			}
		}
	}

	return ret;
}
tBool CConnHandler::ProcessIMsg(void)
{
	if (!_imsg)
	{
		if (!(_imsg = _imsg_que.Pop()))
			return FALSE;
	}

	switch (_imsg->tom)
	{
	case E_WAS_MSG_LOGIN_REQ:
		ProcessLoginREQMsg(_imsg);
		break;
	case E_WAS_MSG_INVITEPEER_REQ:
		ProcessInviteVoiceREQMsg(_imsg);
		break;
	case E_WAS_MSG_INVITEPEER_CNF:
		ProcessInviteVoiceCNFMsg(_imsg);
		break;
	case E_WAS_MSG_ADDMEMBER_CNF:
	case E_WAS_MSG_TRANSFER_CNF:
		break;
	default:
		break;
	}

	_imsg = NULL;

	//ReleaseBridgeMsg(_imsg);

	return TRUE;
}
tBool CConnHandler::DoOTask(void)
{
	return FALSE;
}
tUInt32	CConnHandler::GetASocketId(void)
{
	return (_socket_id_counter=(++_socket_id_counter)%(2<<(32-MSGID_IBTS)));
}
TcpBridgeNet* CConnHandler::GetMyOwnBridgeNet(tUInt32 id) const
{
	tIdBridgeNetConstIter it = _id_bridgenet_map.find(id);

	if (it==_id_bridgenet_map.end())
		return NULL;

	return it->second;
}
void CConnHandler::SaveMyOwnBridgeNet(tUInt32 id,TcpBridgeNet* bridgenet)
{
	//remove the old if any
	tIdBridgeNetIter it = _id_bridgenet_map.find(id);

	if (it!=_id_bridgenet_map.end())
	{
		delete it->second;
		_id_bridgenet_map.erase(it);
	}

	_id_bridgenet_map.insert(tIdBridgeNetPair(id,bridgenet));
}
void CConnHandler::RemoveMyOwnBridgeNet(tUInt32 id)
{
	tIdBridgeNetIter it = _id_bridgenet_map.find(id);

	if (it==_id_bridgenet_map.end())
		return;

	delete it->second;

	_id_bridgenet_map.erase(it);
}
void CConnHandler::ProcessLoginREQMsg(tBridgeMsg* imsg)
{
	tByte* msg;
	tUInt32 len;
	tUInt32 rspc;

	tUInt32 id=0;
	tUInt32 internal_port=0;
	tChar* id_str;

	LOGDEBUG("CConnHandler::ProcessLoginREQMsg:a new user login...");

	if (CanContainMore())
	{
		
		id_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
		tChar* name = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYNM);
		tChar* internal_host = GetDataFromMsg(imsg,E_WAS_TAG_ID_IHOT);
		tChar* internal_port_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_IPOT);

		if (id_str) id = atoi(id_str);
		if (internal_port_str) internal_port = atoi(internal_port_str);

		if (!id || !name || !internal_host || !internal_port)
		{
			LOGWARNING("CConnHandler::ProcessLoginREQMsg:Failed to extract enough from imsg.");

			rspc = X_WAS_RSPC_BAD_REQUEST;
		}
		else
		{
			LOGDEBUG("CConnHandler::ProcessLoginREQMsg:'"<<name<<"' #"<<id<<" login successfully!");

			rspc = X_WAS_RSPC_OK;

			/** move the socket into logined userid and socket map.*/
			tIdSocketIter it = _id_socket_map.find(imsg->tid);
			TcpSocket* socket = it->second;
			_logined_id_socket_map.insert(tIdSocketPair(id,socket));
			_id_socket_map.erase(it);

			CacherDB::GetInstance()->LoginUser(id,name,imsg->rhost,imsg->rport,internal_host,internal_port);
		}

		msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_LOGIN_CNF,imsg->tid, rspc,
							E_WAS_TAG_ID_MBID,strlen(id_str),id_str,
							E_WAS_TAG_ID_NONE);
	}
	else
	{
		LOGDEBUG("CConnHandler::ProcessLoginREQMsg:As a CentralS,currently I am very busy!");

		tUInt32 sid;
		string shost;
		tUInt32 sport;
		if (!_manager->FindMostLeisureSlave(sid,shost,sport))
		{
			LOGDEBUG("CConnHandler::ProcessLoginREQMsg:I found no else leisure Slave CentralS!just turn the login request down.");

			rspc = X_WAS_RSPC_CENTRALS_BUSY;
			msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_LOGIN_CNF,imsg->tid, rspc,
								E_WAS_TAG_ID_MBID,strlen(id_str),id_str,
								E_WAS_TAG_ID_NONE);
		}
		else
		{
			LOGDEBUG("CConnHandler::ProcessLoginREQMsg:Found a Slave CentralS at '"<<shost);

			rspc = X_WAS_RSPC_CENTRALS_FORWARDED;

			tChar sid_str[10]={0};
			sprintf_s(sid_str,10,"%s",sid);
			tChar sport_str[10]={0};
			sprintf_s(sport_str,10,"%s",sport);
			msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_LOGIN_CNF,imsg->tid, rspc,
								E_WAS_TAG_ID_MBID,strlen(id_str),id_str,
								E_WAS_TAG_ID_SSID, strlen(sid_str), sid_str, 
								E_WAS_TAG_ID_PHOT, shost.length(), shost.c_str(),
								E_WAS_TAG_ID_PPOT, strlen(sport_str), sport_str, 
								E_WAS_TAG_ID_NONE);
		}
	}

	if (!msg)
	{
		LOGWARNING("CConnHandler::ProcessLoginREQMsg:Error happened while was_write_msg(...).");
		return;
	}


	tBridgeMsg* bmsg = new tBridgeMsg();

	bmsg->lom = len;
	bmsg->mpi = E_WAS_MSG_FIRST_PART;
	bmsg->tid = imsg->tid;
	bmsg->rspc= rspc;
	bmsg->msg = msg;
	bmsg->tom = E_WAS_MSG_LOGIN_CNF;

	SaveOMsg(bmsg);
}
void CConnHandler::ProcessInviteVoiceREQMsg(tBridgeMsg* imsg)
{
	tUserInfo* userinfo;
	tBridgeMsg* bmsg;
	tByte* msg;
	tUInt32 len;
	tUInt32 rspc=X_WAS_RSPC_NONE;

	tUInt32 myid=0;
	tUInt32 memid=0;

	tChar* myid_str;
	tChar* memid_str;

	LOGDEBUG("CConnHandler::ProcessInviteVoiceREQMsg:processing a invite peer request.");

	myid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	memid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MBID);
	if (myid_str)	myid = atoi(myid_str);
	if (memid_str)	memid = atoi(memid_str);

	if (!myid || !memid)
	{
		LOGWARNING("CConnHandler::ProcessInviteVoiceREQMsg:Failed to extract enough from imsg.");

		rspc = X_WAS_RSPC_BAD_REQUEST;
	}/** check if the destinate member is offline.*/
	else if (!FindSocket(memid))
	{
		LOGWARNING("CConnHandler::ProcessInviteVoiceREQMsg:the member #"<<memid<<" requested is not online.");

		rspc = X_WAS_RSPC_MEMBER_UNAVAILABLE;
	}
	else if (!(userinfo=CacherDB::GetInstance()->GetUserInfo(memid)))
	{
		LOGWARNING("CConnHandler::ProcessInviteVoiceREQMsg:the member #"<<memid<<" requested is online,but no userinfo registered.");

		rspc = X_WAS_RSPC_MEMBER_UNAVAILABLE;
	}

	if (rspc!=X_WAS_RSPC_NONE)
	{
		msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_INVITEPEER_REQ,imsg->tid, rspc,
							E_WAS_TAG_ID_MBID, strlen(myid_str), myid_str, 
							E_WAS_TAG_ID_NONE);

		bmsg = new tBridgeMsg();

		bmsg->lom = len;
		bmsg->mpi = E_WAS_MSG_FIRST_PART;
		bmsg->tid = imsg->tid;
		bmsg->rspc= X_WAS_RSPC_BAD_REQUEST;
		bmsg->msg = msg;
		bmsg->tom = E_WAS_MSG_INVITEPEER_CNF;

		ReleaseBridgeMsg(imsg);
	}
	else
	{
		tChar public_port_str[10]={0};
		tChar internal_port_str[10]={0};
		sprintf_s(public_port_str,10,"%d",userinfo->public_udp_port);
		sprintf_s(internal_port_str,10,"%d",userinfo->internal_udp_port);

		imsg->msg = was_append_to_msg(&len, imsg->msg, 
			E_WAS_TAG_ID_MYNM, userinfo->name.length(), userinfo->name.c_str(), 
			E_WAS_TAG_ID_PHOT, userinfo->public_ip.length(), userinfo->public_ip.c_str(), 
			E_WAS_TAG_ID_PPOT, strlen(public_port_str), public_port_str, 
			E_WAS_TAG_ID_IHOT, userinfo->internal_ip.length(), userinfo->internal_ip.c_str(),
			E_WAS_TAG_ID_IPOT, strlen(internal_port_str), internal_port_str, 
			E_WAS_TAG_ID_NONE);

		imsg->lom = len;

		bmsg = imsg;
	}

	SaveOMsg(bmsg);
}
void CConnHandler::ProcessInviteVoiceCNFMsg(tBridgeMsg* imsg)
{
	tUserInfo* userinfo;
	tBridgeMsg* bmsg;
	tByte* msg;
	tUInt32 len;
	tUInt32 rspc=X_WAS_RSPC_OK;

	tUInt32 myid=0;
	tUInt32 memid=0;

	tChar* myid_str;
	tChar* memid_str;

	LOGDEBUG("STConn::ProcessInviteVoiceCNFMsg:confirmation returned from dest member.");

	/** If not successfully accepted by peer,just forward the rejection.*/
	if (imsg->rspc!=X_WAS_RSPC_OK)
	{
		SaveOMsg(imsg);
		return ;
	}

	myid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MYID);
	memid_str = GetDataFromMsg(imsg,E_WAS_TAG_ID_MBID);
	if (myid_str)	myid = atoi(myid_str);
	if (memid_str)	memid = atoi(memid_str);

	if (!myid || !memid)
	{
		LOGWARNING("CConnHandler::ProcessInviteVoiceREQMsg:Failed to extract enough from imsg.");

		rspc = X_WAS_RSPC_BAD_REQUEST;
	}/** check if the destinate member is offline.*/
	else if (!FindSocket(memid))
	{
		LOGWARNING("CConnHandler::ProcessInviteVoiceREQMsg:the member #"<<memid<<" requested is not online.");

		rspc = X_WAS_RSPC_MEMBER_UNAVAILABLE;
	}
	else if (!(userinfo=CacherDB::GetInstance()->GetUserInfo(memid)))
	{
		LOGWARNING("CConnHandler::ProcessInviteVoiceREQMsg:the member #"<<memid<<" requested is online,but no userinfo registered.");

		rspc = X_WAS_RSPC_MEMBER_UNAVAILABLE;
	}

	if (rspc!=X_WAS_RSPC_OK)
	{
		msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_INVITEPEER_CNF,imsg->tid, rspc,
							E_WAS_TAG_ID_MBID, strlen(myid_str), myid_str, 
							E_WAS_TAG_ID_NONE);

		bmsg = new tBridgeMsg();

		bmsg->lom = len;
		bmsg->mpi = E_WAS_MSG_FIRST_PART;
		bmsg->tid = imsg->tid;
		bmsg->rspc= rspc;
		bmsg->msg = msg;
		bmsg->tom = E_WAS_MSG_INVITEPEER_CNF;

		ReleaseBridgeMsg(imsg);
	}
	else
	{
		tChar public_port_str[10]={0};
		tChar internal_port_str[10]={0};
		sprintf_s(public_port_str,10,"%d",userinfo->public_udp_port);
		sprintf_s(internal_port_str,10,"%d",userinfo->internal_udp_port);

		imsg->msg = was_append_to_msg(&len, imsg->msg, 
			E_WAS_TAG_ID_MYNM, userinfo->name.length(), userinfo->name.c_str(), 
			E_WAS_TAG_ID_PHOT, userinfo->public_ip.length(), userinfo->public_ip.c_str(), 
			E_WAS_TAG_ID_PPOT, strlen(public_port_str), public_port_str, 
			E_WAS_TAG_ID_IHOT, userinfo->internal_ip.length(), userinfo->internal_ip.c_str(),
			E_WAS_TAG_ID_IPOT, strlen(internal_port_str), internal_port_str, 
			E_WAS_TAG_ID_NONE);

		imsg->lom = len;

		bmsg = imsg;
	}

	SaveOMsg(bmsg);
}
TcpSocket* CConnHandler::FindSocket(tUInt32 id) const
{
	tIdSocketConstIter it = _logined_id_socket_map.find(id);

	if (it==_logined_id_socket_map.end())
		return NULL;
	else
		return it->second;
}