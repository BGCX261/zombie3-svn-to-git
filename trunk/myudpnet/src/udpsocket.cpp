#include "udpsocket.hxx"
#include "priv/udpsocketimp.hxx"
//#include "myfilelog.hxx"
//#include "loghelper.hxx"

#include <string>
#include <sstream>

UdpSocket::UdpSocket()
		:_udpsocketimp()
{
	_udpsocketimp = new UdpSocketImp();
}
UdpSocket::~UdpSocket()
{
	if(_udpsocketimp)
	{
		delete _udpsocketimp;
		_udpsocketimp = NULL;
	}
}
tInt32 UdpSocket::SendTo(tByte* data, tInt32 size,string host_remote, tInt32 port_remote)
{
	return _udpsocketimp->SendTo(data,size,host_remote,port_remote);
}
tInt32 UdpSocket::SendTo(tByte* data, tInt32 size,const sockaddr_in* sockaddr_remote)
{
	return _udpsocketimp->SendTo(data,size,sockaddr_remote);
}
tInt32 UdpSocket::RecvFrom(tByte* buf, tInt32 bufsize,string &host_remote, tInt32 &port)
{
	return _udpsocketimp->RecvFrom(buf,bufsize,host_remote,port);
}
int UdpSocket::IsReadyForRead(unsigned int ms)
{
	return _udpsocketimp->IsReadyForRead(ms);
}
int UdpSocket::IsReadyForWrite()
{
	return _udpsocketimp->IsReadyForWrite();
}
bool UdpSocket::SetReuseAddr()
{
	return _udpsocketimp->SetReuseAddr();
}
bool UdpSocket::ClearReuseAddr()
{
	return _udpsocketimp->ClearReuseAddr();
}
SOCKET UdpSocket::GetHandle()
{
	return _udpsocketimp->GetHandle();
}
void UdpSocket::Close()
{
	return _udpsocketimp->Close();
}
unsigned short UdpSocket::PeerLocalPort() const
{
	return _udpsocketimp->PeerLocalPort();
}
string UdpSocket::PeerLocalHost() const
{
	return _udpsocketimp->PeerLocalHost();
}