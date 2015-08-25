#include "priv/groupmgr.hxx"
#include "tcpsocket.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "tcpsocket.hxx"
#include "tcpbridgenet.hxx"
#include "was_api.h"
#include "mytimer.hxx"
#include "bridgenet.hxx"	//DuplicateMsg
#include "priv/itconn.hxx"

ITConn::ITConn(ITConnManager* connmanager,TcpSocket* remote,FileLog *filelog)
  : TConn("",0,filelog),_tfile_list(NULL),_manager(connmanager)
{
	/* put it into prerunning.*/
	//_bridgenet = new TcpBridgeNet(filelog);
	//_bridgenet->SetTcpSocket(remote);

	//_remote_host = remote->PeerHostAddress();
	//_remote_port = remote->PeerHostPort();

}
ITConn::~ITConn()
{
}
void ITConn::ProcessIMsgTransferReqest(tBridgeMsg* imsg)
{
	//LOGINFO("ITConn::ProcessIMsgInvitePeer:receive a request to transfer data from "<<_bridgenet->PeerHostAddress());

	///**
	// * register this request first
	// */
	//tTStatus* status = new tTStatus();

	//status->name = "TransferDataRequest";
	//status->state = OT_STATE_REQUEST_REQUESTED;
	//status->description = "Request to transfer data to me.";
	//reqestmsg = DuplicateMsg(imsg);

	//RegisterStatus(status);
}
void ITConn::ProcessIMsgTransferData(tBridgeMsg* imsg)
{
	//tTStatus* status = GetTStatusByTaskId(imsg->tid);

	//if (!status)
	//{
	//	LOGWARNING("ITConn::ProcessIMsgTransferData:found no status for tid "<<imsg->tid);
	//	return;
	//}
}
tBool ITConn::ProcessIMsg(void)
{
	if (!_imsg)
	{
		if (!(_imsg = _imsg_que.Pop()))
			return FALSE;
	}

	switch (_imsg->tom)
	{
	case E_WAS_MSG_TRANSFER_REQ:
		ProcessIMsgTransferReqest(_imsg);
		break;
	case E_WAS_MSG_TRANSFER_DATA:
		ProcessIMsgTransferData(_imsg);
		break;
	default:
		LOGWARNING("ITConn::ProcessIMsg:got a unknown imsg with tom "<<_imsg->tom);
		break;
	}

	/**
	 * release the processed imsg,the status has its own copy of the imsg
	 */
	ReleaseBridgeMsg(&_imsg);

	return TRUE;
}

tBool ITConn::DoOTask(void)
{
	tBool ret = FALSE;

	return ret;
}
void ITConn::RegisterTransferingFile(tTransFile* tfile)
 {
	 if (!_tfile_list)
		_tfile_list = tfile;
	 else
	 {
		 tfile->next = _tfile_list->next;
		 _tfile_list = tfile;
	 }
 }
tTransFile* ITConn::GetTransferingFile(tUInt32 tid)
{
	tTransFile* ret_tfile = NULL;
	tTransFile* tfile = _tfile_list;

	while (tfile)
	{
		if (tfile->id == tid)
		{
			ret_tfile = ret_tfile;
			break;
		}
		else
			tfile = tfile->next;
	}

	return ret_tfile;
}