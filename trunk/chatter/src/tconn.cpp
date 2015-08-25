#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "priv/tconn.hxx"

tUInt32 TConn::_tid_counter = 1;

TConn::TConn(string remote_host,tUInt32 remote_port,FileLog* filelog)
:	Thread(),
	_remote_host(remote_host),
	_remote_port(remote_port),
	_filelog(filelog),
	_imsg_que(),
	_omsg_que(),
	_imsg(NULL),
	_omsg(NULL),
	_is_shutdown_requested(FALSE)
{}
TConn::~TConn()
{
}
void TConn::Shutdown(void)
{
	_is_shutdown_requested = TRUE;
}
string TConn::GetRemoteHost(void) const
{
	return _remote_host;
}
tUInt32 TConn::GetRemotePort(void) const
{
	return _remote_port;
}
void TConn::Execute(Thread::Arg arg)
{
	tBool iret;
	tBool oret;

	LOGDEBUG("TConn::Execute #"<<Id()<<" started...");

	PreRunning();

	while (!_is_shutdown_requested)
	{
		iret = HandleIncomming();
		oret = HandleOutgoing();

		if (!iret && !oret)
		{
			Yield();
		}
	}

	AfterRunning();

	/*
	 * Here received a shutdown singnal
	 */

	LOGDEBUG("TConn::Execute #"<<Id()<<" ended.");
}
tBool TConn::HandleIncomming(void)
{
	tBool recv_ret;
	tBool prci_ret;

	recv_ret = RecvBridgeMsg();
	prci_ret = ProcessIMsg();

	return (recv_ret || prci_ret);
}
tBool TConn::HandleOutgoing(void)
{
	tBool dot_ret;
	tBool send_ret;

	dot_ret = DoOTask();
	send_ret = SendBridgeMsg();

	return (dot_ret || send_ret);
}
tBool TConn::RecvBridgeMsg(void)
{
	//tBool ret = FALSE;

	//tBridgeMsg* imsg;

	//TcpBridgeNet::tBridgeNetStatus res;

	//res=_bridgenet->ReceiveMsg(&imsg);

	//switch(res)
 //   {
	//case TcpBridgeNet::eMsgNotAvailable:
	//	break;
	//case TcpBridgeNet::eMsgAvailable:
	//	{
	//		LOGDEBUG("TConn::RecvBridgeMsg:Connection #"<<Id()<<" received a imsg from peer '"<<_remote_host<<"'.");
	//		ret = TRUE;
	//		SaveIMsg(imsg);
	//	}
	//	break;
 //   case TcpBridgeNet::eEndOfConnectionReceived:
	//	{
	//		LOGDEBUG("TConn::RecvBridgeMsg:Connection #"<<Id()<<" received end of connection signal from peer '"<<_remote_host<<"'.");
	//		
	//		Shutdown();
	//	}
	//	break;

 //   case TcpBridgeNet::eNetworkErrorReceived:
	//	{
	//		LOGWARNING("TConn::RecvBridgeMsg:Connection #"<<Id()<<" received network error from '"<<_remote_host<<"'.");
	//        
	//		Shutdown();
	//	}
	//	break;

 //   default:
	//	{
	//		LOGERROR("TConn::RecvBridgeMsg:Execute Connection #"<<Id()<<" Unexpected returned value "<<res<<" from '"<<_remote_host<<"'.");
	//	}
	//	break;
 //   } // switch(res)

	//return ret;
	return TRUE;
}
tBool TConn::SendBridgeMsg(void)
{
	//if (!_omsg)
	//{
	//	if (!(_omsg = _omsg_que.Pop()))
	//		return FALSE;
	//}

	//assert(_bridgenet);

	//if (!_bridgenet->IsReadyForWrite())
	//	return FALSE;

	//if(!_bridgenet->SendBridgeMsg(_omsg))
	//{
	//	LOGWARNING("TConn::SendBridgeMsg:Thread #"<<Id()<<" failed to send the omsg.");
	//	return FALSE;
	//}
	//else
	//{
	//	LOGDEBUG("TConn::SendBridgeMsg:omsg has been successfully sent to peer.");

	//	/** Here updating status required, for it's the end of the omsg. */
	//	tTStatus* status = GetTStatusByTaskId(_omsg->tid);
	//	if (status)
	//	{
	//		if (status->state==OT_STATE_REQUEST_CREATED)
	//			status->state=OT_STATE_REQUEST_SENT;
	//	}

	//	ReleaseBridgeMsg(&_omsg);
	//}

	return TRUE;
}

void TConn::PreRunning(void)
{}
void TConn::AfterRunning(void)
{}
tBool TConn::ProcessIMsg(void)
{
	return FALSE;
}
tBool TConn::DoOTask(void)
{
	return FALSE;
}
void TConn::Disconnect(void)
{
}


//void TConn::GetMemInfoFromMsg(tBridgeMsg* msg,tMemInfo** meminfo)
//{
//	(*meminfo) = new tMemInfo;
//
//	tChar* mem_name_str  = GetDataFromMsg(msg,E_WAS_TAG_ID_MBNM);
//	tChar* mem_id_str	 = GetDataFromMsg(msg,E_WAS_TAG_ID_MBID);
//	tChar* mem_phost_str = GetDataFromMsg(msg,E_WAS_TAG_ID_PHOT);
//	tChar* mem_pport_str = GetDataFromMsg(msg,E_WAS_TAG_ID_PPOT);
//	tChar* mem_ihost_str = GetDataFromMsg(msg,E_WAS_TAG_ID_IHOT);
//	tChar* mem_iport_str = GetDataFromMsg(msg,E_WAS_TAG_ID_IPOT);	
//
//	if (mem_name_str)	(*meminfo)->name			=string(mem_name_str);
//	if (mem_id_str)		(*meminfo)->id				=(tUInt32)atoi(mem_id_str);
//	if (mem_phost_str)	(*meminfo)->public_ip		=string(mem_phost_str);
//	if (mem_pport_str)	(*meminfo)->public_port		=(tUInt32)atoi(mem_pport_str);
//	if (mem_ihost_str)	(*meminfo)->internal_ip		=string(mem_phost_str);
//	if (mem_iport_str)	(*meminfo)->internal_port	=(tUInt32)atoi(mem_pport_str);
//
//	if (mem_name_str)	delete mem_name_str;
//	if (mem_id_str)		delete mem_id_str;
//	if (mem_phost_str)	delete mem_phost_str;
//	if (mem_pport_str)	delete mem_pport_str;
//	if (mem_ihost_str)	delete mem_phost_str;
//	if (mem_iport_str)	delete mem_pport_str;
//}

void TConn::SaveOMsg(tBridgeMsg* bmsg)
{
	//assert(bmsg!=NULL);

	_omsg_que.Push(bmsg);
}
void TConn::SaveIMsg(tBridgeMsg* bmsg)
{
	//assert(bmsg!=NULL);

	_imsg_que.Push(bmsg);
}
void TConn::ClearTConn(void)
{
	ClearOMsgQue();

	ClearIMsgQue();
}
tUInt32	TConn::GetATId(void)
{
	return (_tid_counter++);
}
void TConn::ClearOMsgQue(void)
{
	ReleaseBridgeMsg(&_omsg);

	while (_omsg = _omsg_que.Pop())
	{
		ReleaseBridgeMsg(&_omsg);
	}
}
void TConn::ClearIMsgQue(void)
{
	ReleaseBridgeMsg(&_imsg);

	while (_imsg = _imsg_que.Pop())
	{
		ReleaseBridgeMsg(&_imsg);
	}
}