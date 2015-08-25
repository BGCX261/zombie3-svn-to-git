#include "udpserversocket.hxx"
#include "priv/udpsocketimp.hxx"

UdpServerSocket::UdpServerSocket(tInt32 port)
: UdpSocket()
{
	Bind(port);
}

UdpServerSocket::UdpServerSocket()
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"UdpServerSocket::UdpServerSocket(): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

UdpServerSocket::UdpServerSocket(const UdpServerSocket &model)
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"UdpServerSocket::UdpServerSocket(const UdpServerSocket &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

UdpServerSocket::~UdpServerSocket()
{
	Close();
}
tInt32 UdpServerSocket::Bind(tInt32 port)
{
	return _udpsocketimp->Bind(port);
}