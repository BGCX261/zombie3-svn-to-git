#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "tcpclientsocket.hxx"
#include "socketexception.hxx"
#include "was_api.h"
#include "bridgenet.hxx"
#include "tcpbridgenet.hxx"
#include "priv/otconnmanager.hxx"
#include "priv/ptconn.hxx"

PTConn::PTConn(OTConnManager* connmanager,string remote_public_host,tUInt32 remote_public_port,
				string remote_internal_host,tUInt32 remote_internal_port,FileLog* filelog)
:	TConn(remote_public_host,remote_public_port,filelog),
	_manager(connmanager),
	_remote_internal_host(remote_internal_host),
	_remote_internal_port(remote_internal_port)
{}

PTConn::~PTConn()
{
}
TcpBridgeNet* PTConn::PHoleBuildBridgeNet(void)
{
	//TcpClientSocket* socket = NULL;

	///**
	// * Create a TcpSocket for the tcpbridgenet
	// */
	//try
	//{
	//	socket = new TcpClientSocket();
	//}
	//catch (SocketException& e)
	//{
	//	LOGWARNING("TConn::BuildBridgeNet:SocketException happened when creating a TcpClientSocket:"<<e.Description());

	//	if (socket) 
	//	{
	//		delete socket;
	//		socket = NULL;
	//	}

	//	return NULL;
	//}

	///**
	// * connect to reomte host with the socket
	// */
	//if (socket->Connect(_remote_host, _remote_port) != 0)
	//{
	//	LOGWARNING("TConn::BuildBridgeNet:Failed to connect to "<<_remote_host<<"::"<<_remote_port);
	//	
	//	if (socket) 
	//	{
	//		delete socket;
	//		socket = NULL;
	//	}
	//	return NULL;
	//}

	///**
	// * Once connected, we successfully made the tcpbridgenet
	// */
	//_bridgenet = new TcpBridgeNet(socket, _filelog);

	//return _bridgenet;
	return NULL;
}
tUInt32 PTConn::Transfer(string file_path_and_name)
{
	/**
	 * Register the task, and leave the transmission work to DoTask(...)
	 */
	tUInt32 tid = GetATId();

	return tid;
}
tBool PTConn::ProcessIMsg(void)
{
	if (!_imsg)
	{
		if (!(_imsg = _imsg_que.Pop()))
			return FALSE;
	}

	switch (_imsg->tom)
	{
	default:
		break;
	}

	return TRUE;
}
tBool PTConn::DoOTask(void)
{
	tBool ret = FALSE;

	return ret;
}