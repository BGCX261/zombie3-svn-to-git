#include "tcpsocket.hxx"
#include "priv/sconnhandler.hxx"

SConnHandler::SConnHandler(SConnManager* manager,TcpSocket* socket,FileLog* filelog)
:	ConnHandler(filelog),
	_manager(manager),
	_socket(socket),
	_slave_id(0),
	_slave_host(),
	_slave_port(0),
	_load(0)
{
	if (socket)
		_slave_host = socket->PeerHostAddress();
}
SConnHandler::~SConnHandler()
{
}
tUInt32 SConnHandler::GetSlaveId(void) const
{
	return _slave_id;
}
string SConnHandler::GetSlaveHost(void) const
{
	return _slave_host;
}
tUInt32 SConnHandler::GetSlavePort(void) const
{
	return _slave_port;
}
tUInt32 SConnHandler::GetSlaveLoad(void) const
{
	return _load;
}
void SConnHandler::PreRunning(void)
{}
void SConnHandler::AfterRunning(void)
{}
tBool SConnHandler::ProcessIMsg(void)
{
	return FALSE;
}
tBool SConnHandler::DoOTask(void)
{
	return FALSE;
}