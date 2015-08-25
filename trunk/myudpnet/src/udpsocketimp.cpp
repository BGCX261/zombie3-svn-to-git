#include <sstream>
#include <string>

#include "priv/udpsocketimp.hxx"

// The following is used by WSAStartup but I didn't find any document
// describing the usage of this variable (to be frank I didn't really
// searched a lot).
static WSADATA wsaData;

// The following counter allows us to know when to initialize the Winsock DLL
// and when to release it.
static int  _socket_counter=0;

int InitLib()
{
  int res = WSAStartup(MAKEWORD(2,2), &wsaData);

  return res;
}

void ReleaseLib()
{
  WSACleanup();
}

UdpSocketImp::UdpSocketImp():_socket(INVALID_SOCKET),_sockaddr_remote()
{
	tInt32 socket_return = 0;

	socket_return = Socket();
}

UdpSocketImp::~UdpSocketImp()
{
	Close();
}
tInt32 UdpSocketImp::Socket()
{
	tInt32 socket_return = 0;

	if (_socket_counter == 0)
	{
		int lib_return=InitLib();

		if (lib_return!=0)
		{
		  ostringstream ostr;
		  ostr<<"TcpSocketImpl::TcpSocketImpl: 'WSAStartup' returned the error code "<<lib_return<<".";

		  throw UdpSocketException(ostr.str());
		}
    }

	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (_socket == INVALID_SOCKET)
	{		
		socket_return = WSAGetLastError();
		ostringstream ostr;
		ostr<<"UdpSocketImp::Socket: 'socket' failed with error code "<<socket_return<<".";
		throw UdpSocketException(ostr.str());
	}

	_socket_counter++;

	return socket_return;
}
tInt32 UdpSocketImp::SetBroadcast()
{
	// Set the access,theoretically it's not necessary,but sometimes,especially
	// On my OWN pc, it requires, or error 10013 raised.
	BOOL     bBroadcast=TRUE;    
	return setsockopt(_socket,SOL_SOCKET,SO_BROADCAST,(const   char*)&bBroadcast,sizeof(BOOL));   
}
tInt32 UdpSocketImp::Bind(tInt32 port)
{
	tInt32 bind_return = 0;

	sockaddr_in address_me;

	memset((char *) &address_me, 0x00, sizeof(address_me));

    address_me.sin_family = AF_INET;
    address_me.sin_port = htons(port);
    address_me.sin_addr.s_addr = htonl(INADDR_ANY); 

	bind_return = bind(_socket, (SOCKADDR*)&address_me, sizeof(address_me));

	if (bind_return == SOCKET_ERROR)
	{
		bind_return = WSAGetLastError();
		ostringstream ostr;
		ostr<<"UdpSocketImp::Socket: Bind to port '"<<port<<"' failed with error code "<<bind_return<<".";
		throw UdpSocketException(ostr.str());
	}

	return bind_return;
}
tInt32 UdpSocketImp::SendTo(tByte* data, tInt32 size,string host_remote, tInt32 port_remote)
{
	tInt32 send_return = 0;

	struct sockaddr_in address_remote;

	address_remote.sin_family = AF_INET;
	address_remote.sin_port = htons(port_remote);

	tInetAddr inetaddr=inet_addr(host_remote.c_str());

  if (inetaddr==INADDR_NONE)
    {
      // host isn't an IP address (dot notation) so we have to resolve
      // the name with DNS.
      struct hostent* phost = gethostbyname(host_remote.c_str());

      if (phost != 0)
	  {
	    inetaddr = *((tInetAddr *)phost->h_addr_list[0]);
	  }
    }

	memcpy((void *)&address_remote.sin_addr.s_addr, (void *)&inetaddr, sizeof(inetaddr));

	//address_remote.sin_addr.s_addr = inet_addr(host_remote.c_str());

	send_return = SendTo(data,size,&address_remote);

	if (SOCKET_ERROR == send_return)
	{
		int error_code = WSAGetLastError();
	}

	return send_return;
}
tInt32 UdpSocketImp::SendTo(tByte* data, tInt32 size,const sockaddr_in* address_remote)
{
	tInt32 send_return = 0;

	send_return = sendto(_socket,(char*)data,size,0,(SOCKADDR *)address_remote,sizeof(*((SOCKADDR *)address_remote)));

	return send_return;
}
//tInt32 UdpSocketImp::SendTo(tByte* data, tInt32 size)
//{
//	tInt32 send_return = 0;
//
//	send_return = sendto(_socket,(char*)data,size,0,(SOCKADDR *)address_remote,sizeof(*((SOCKADDR *)address_remote)));
//
//	return send_return;
//}
tInt32 UdpSocketImp::RecvFrom(tByte* buf, tInt32 bufsize, string &host_remote, tInt32 &port_remote)
{
	tInt32 recv_return = 0;

	tInt32 sendaddrsize = sizeof(_sockaddr_remote);

	recv_return = recvfrom(_socket,(char*)buf,bufsize,0,(SOCKADDR *)&_sockaddr_remote,&sendaddrsize);

	if (SOCKET_ERROR != recv_return)
	{
		host_remote.assign(inet_ntoa(_sockaddr_remote.sin_addr));

		port_remote = ntohs(_sockaddr_remote.sin_port);
	}
	else
	{
		int error_code = WSAGetLastError();
	}

	return recv_return;
}
int UdpSocketImp::IsReadyForRead(unsigned int ms)
{
  int retval;  
#if defined(linux)

  struct pollfd fdsock;
  fdsock.fd=_socket;   
  // Following line can be used for some specific need which I don't 
  // clearly understand ... 
  //fdsock.events=POLLIN|POLLPRI|POLLRDNORM|POLLRDBAND;
  // ... this is the reason why we will use this generic one:
  fdsock.events=POLLIN|POLLPRI;
  fdsock.revents=0;

  //Wait fro ms milliseconds
  retval=poll(&fdsock,1,ms);

  if (retval==SOCKET_ERROR)
    {
      retval=errno;
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::IsReadyForRead: 'select' returned the error code "<<retval<<".";
      FileLogDebug(ostr.str());
#endif

      if (retval>0) retval=-retval;
    }
#elif defined(WIN32)

  fd_set readfds;
  struct timeval tv;
  FD_ZERO(&readfds);
  tv.tv_sec=0;
  
  //Convert it to microsecond
  tv.tv_usec=ms*1000;
  FD_SET(_socket,&readfds);
  retval=select(_socket+1,&readfds,NULL,NULL,&tv);

  if (retval==SOCKET_ERROR)
    {
      retval=WSAGetLastError();
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::IsReadyForRead: 'select' returned the error code "<<retval<<".";
      FileLogDebug(ostr.str());
#endif
      if (retval>0) retval=-retval;
    }
#else
#error "TcpSocketImpl::IsReadyForRead has empty implementation on this platform"
#endif
  return retval;

}

int UdpSocketImp::IsReadyForWrite()
{
  int retval;  

#if defined(linux)
  struct pollfd fdsock;
  fdsock.fd=_socket;
  // Following line can be used for some specific need which I don't 
  // clearly understand ... 
  // fdsock.events=POLLOUT|POLLWRNORM|POLLWRBAND;
  // ... this is the reason why we will use this generic one:
  fdsock.events=POLLOUT;
  fdsock.revents=0;  
  retval=poll(&fdsock,1,0);

  if (retval==SOCKET_ERROR)
    {
      retval=errno;
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::IsReadyForWrite: 'select' returned the error code "<<retval<<".";
      FileLogDebug(ostr.str());
#endif
      if (retval>0) retval=-retval;
    }

#elif defined(WIN32)

  fd_set writefds; 
  struct timeval tv;
  FD_ZERO(&writefds);
  tv.tv_sec=0;
  tv.tv_usec=0;
  FD_SET(_socket,&writefds);
  retval=select(_socket+1,NULL,&writefds,NULL,&tv);

  if (retval==SOCKET_ERROR)
    {
      retval=WSAGetLastError();
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::IsReadyForWrite: 'select' returned the error code "<<retval<<".";
      FileLogDebug(ostr.str());
#endif
      if (retval>0) retval=-retval;
    }

#else
#error "TcpSocketImpl::IsReadyForWrite has empty implementation on this platform"
#endif

  return retval;
}

bool UdpSocketImp::SetReuseAddr()
{

  int on = 1;
  int setsockopt_return=setsockopt(_socket,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on));

  if (setsockopt_return==SOCKET_ERROR)
    {
#if defined(linux)
      setsockopt_return=errno;
#elif defined(WIN32)
      setsockopt_return=WSAGetLastError();
#else
#error "TcpSocketImpl::SetReuseAddr: Function won't return the appropriate value in case of error."
#endif

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::SetReuseAddr: 'setsockopt' returned the error code "<<setsockopt_return<<".";
      FileLogDebug(ostr.str());
#endif


    }
  return setsockopt_return==0;
}

bool UdpSocketImp::ClearReuseAddr()
{

  int off = 0;
  int setsockopt_return=setsockopt(_socket,SOL_SOCKET,SO_REUSEADDR,(const char*)&off,sizeof(off));

  if (setsockopt_return==SOCKET_ERROR)
    {
#if defined(linux)
      setsockopt_return=errno;
#elif defined(WIN32)
      setsockopt_return=WSAGetLastError();
#else
#error "TcpSocketImpl::ClearReuseAddr: Function won't return the appropriate value in case of error."
#endif

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::ClearReuseAddr: 'setsockopt' returned the error code "<<setsockopt_return<<".";
      FileLogDebug(ostr.str());
#endif


    }
  return setsockopt_return==0;
}


SOCKET UdpSocketImp::GetHandle()
{
  return _socket;
}

void   UdpSocketImp::Close()
{
	if (INVALID_SOCKET != _socket)
	{
		_socket_counter--;
		closesocket(_socket);
		_socket = INVALID_SOCKET;

		if (_socket_counter == 0)
		{
			ReleaseLib();
		}
	}
}

unsigned short UdpSocketImp::PeerLocalPort() const
{
	struct sockaddr localaddr={0};
	int len = sizeof(sockaddr);

	int res = getsockname ( _socket, &localaddr, &len );

	int error_code = WSAGetLastError();  

	if ( res == SOCKET_ERROR ) 
		return 0;
	else
		return ntohs (((struct sockaddr_in *)&localaddr)->sin_port );
}
string UdpSocketImp::PeerLocalHost() const
{
	char  buffer[100]={0};

	if (gethostname (buffer,100) == SOCKET_ERROR)
	{
	   return "0.0.0.0";
	}

	HOSTENT*  lpHostEnt = gethostbyname(buffer);

	char*  lpAddr = (char*)inet_ntoa(*(LPIN_ADDR)*(lpHostEnt->h_addr_list));

	return string(lpAddr);
}