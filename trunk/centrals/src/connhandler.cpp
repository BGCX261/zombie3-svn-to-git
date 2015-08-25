#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "priv/connhandler.hxx"

ConnHandler::ConnHandler(FileLog* filelog)
:	Thread(),
	_filelog(filelog),
	_imsg_que(),
	_omsg_que(),
	_imsg(NULL),
	_omsg(NULL),
	_is_shutdown_requested(FALSE)
{}
ConnHandler::~ConnHandler()
{
}
void ConnHandler::Shutdown(void)
{
	_is_shutdown_requested = TRUE;
}
void ConnHandler::Execute(Thread::Arg arg)
{
	tBool iret;
	tBool oret;

	LOGDEBUG("ConnHandler::Execute #"<<Id()<<" started...");

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

	/*
	 * Here received a shutdown singnal
	 */

	AfterRunning();

	LOGDEBUG("ConnHandler::Execute #"<<Id()<<" ended.");
}
tBool ConnHandler::HandleIncomming(void)
{
	tBool recv_ret;
	tBool prci_ret;

	recv_ret = RecvBridgeMsg();
	prci_ret = ProcessIMsg();

	return (recv_ret || prci_ret);
}
tBool ConnHandler::HandleOutgoing(void)
{
	tBool dot_ret;
	tBool send_ret;

	dot_ret = DoOTask();
	send_ret = SendBridgeMsg();

	return (dot_ret || send_ret);
}
tBool ConnHandler::RecvBridgeMsg(void)
{
	tBool ret = FALSE;

	return ret;
}
tBool ConnHandler::SendBridgeMsg(void)
{
	//if (!_omsg)
	//{
	//	if (!(_omsg = _omsg_que.Pop()))
	//		return FALSE;
	//}

	return FALSE;
}
void ConnHandler::PreRunning()
{}
void ConnHandler::AfterRunning()
{}
tBool ConnHandler::ProcessIMsg(void)
{
	return FALSE;
}
tBool ConnHandler::DoOTask(void)
{
	return FALSE;
}
//void  ConnHandler::GetMemInfoFromMsg(tBridgeMsg* msg,tMemInfo** meminfo)
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
//	if (mem_pport_str)	(*meminfo)->pport		=(tUInt32)atoi(mem_pport_str);
//	if (mem_ihost_str)	(*meminfo)->internal_ip		=string(mem_phost_str);
//	if (mem_iport_str)	(*meminfo)->iport	=(tUInt32)atoi(mem_pport_str);
//
//	if (mem_name_str)	delete mem_name_str;
//	if (mem_id_str)		delete mem_id_str;
//	if (mem_phost_str)	delete mem_phost_str;
//	if (mem_pport_str)	delete mem_pport_str;
//	if (mem_ihost_str)	delete mem_phost_str;
//	if (mem_iport_str)	delete mem_pport_str;
//}

void ConnHandler::SaveOMsg(tBridgeMsg* bmsg)
{
	assert(bmsg!=NULL);

	_omsg_que.Push(bmsg);
}
void ConnHandler::SaveIMsg(tBridgeMsg* bmsg)
{
	assert(bmsg!=NULL);

	_imsg_que.Push(bmsg);
}
void ConnHandler::ClearTConn(void)
{
	ClearOMsgQue();

	ClearIMsgQue();
}
void ConnHandler::ClearOMsgQue(void)
{
	ReleaseBridgeMsg(&_omsg);

	while (_omsg = _omsg_que.Pop())
	{
		ReleaseBridgeMsg(&_omsg);
	}
}
void ConnHandler::ClearIMsgQue(void)
{
	ReleaseBridgeMsg(&_imsg);

	while (_imsg = _imsg_que.Pop())
	{
		ReleaseBridgeMsg(&_imsg);
	}
}