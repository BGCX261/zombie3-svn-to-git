#include "tcpsocket.hxx"
#include "socketexception.hxx"
#include "priv/tcpsocketimpl.hxx"
//#include "loghelper.hxx"

#include <string>
#include <sstream>
using namespace std;

#if defined(WIN32)
#include <process.h>
#elif defined(linux)
#include <sys/types.h>
#include <unistd.h>
#else
#error "Unsupported platform"
#endif

#if defined(SOCKETDEBUG)
#include "conversion.h"
#include "myfilelog.hxx"
static FileLog thefilelog;
static FileLog *_filelog=&thefilelog;
static bool _filelog_initialized=false;

#define FileLogDebugDefined
#define FileLogDebug(x)   if (1) {	      \
    ostringstream *ostr;                      \
    ostr=new ostringstream();                 \
    (*ostr)<<x;                               \
    if (!_filelog_initialized)                \
    {                                         \
      stringstream ss;                        \
      ss<<"log/mynet-"<<getpid()<<".log";     \
      _filelog_initialized=true;              \
      _filelog->SetFileName(ss.str());        \
      _filelog->SetMode(true);                \
    }                                         \
    _filelog->AppendDebug(ostr->str());	      \
    delete ostr;                              \
}
#endif

// Following include is for memset.
#include <string.h>

#define MAXCONNECTIONS  1000
#define MAXRECV 2000

#if defined(linux)

// Nothing to do for Linux here.

#elif defined(WIN32)

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

#else
#error "TcpSocket class is unsupported on this platform."
#endif

TcpSocketImpl::TcpSocketImpl()
  : _socket(INVALID_SOCKET), _socket_addr(), _connected(false), _boundhost()
{
  int socket_return=Socket();
  if (socket_return!=0)
    {
      ostringstream ostr;
#if defined(DEBUG)
      ostr<<"TcpSocketImpl::TcpSocketImpl(): 'socket' returned the error code "<<socket_return<<".";
#else
      ostr<<"'socket' returned the error code "<<socket_return<<".";
#endif
#if defined(DEBUG) && defined(FileLogDebugDefined)
      FileLogDebug(ostr.str());
#endif
      throw SocketException(ostr.str());
    }

  bool setsockopt_return=SetReuseAddr();
  if (!setsockopt_return)
    {
      ostringstream ostr;
#if defined(DEBUG)
      ostr<<"TcpSocketImpl::TcpSocketImpl(int port): 'SetReuseAddr' returned the error code "<<setsockopt_return<<".";
#else
      ostr<<"'SetReuseAddr' returned the error code "<<setsockopt_return<<".";
#endif
#if defined(DEBUG) && defined(FileLogDebugDefined)
      FileLogDebug(ostr.str());
#endif
      throw SocketException(ostr.str());
    }
}

TcpSocketImpl::TcpSocketImpl(SOCKET socket, sockaddr_in &socket_addr, bool connected) 
  : _socket(socket),_socket_addr(socket_addr), _connected(connected), _boundhost()
{
#if defined(WIN32)
  // WARNING FOR WINDOWS USAGE: 
  // This constructor should be called by TcpSocketImpl only and
  // so the library should have been already initialized when creating 
  // the server socket (with the constructor above). If any other usage 
  // of this constructor is performed, then there is a risk of runtime 
  // error for uninitialized library because the given parameters 
  // 'socket' and 'socket_addr' require to call the Winsock functions 
  // that themselves requires the library to be initialized first. 
  _socket_counter++;
#endif
  _boundhost=inet_ntoa(_socket_addr.sin_addr);
}

TcpSocketImpl::TcpSocketImpl(int port)
  : _socket(INVALID_SOCKET), _socket_addr(), _connected(false), _boundhost()
{
  int socket_return=Socket();
  if (socket_return!=0)
    {
      ostringstream ostr;
#if defined(DEBUG)
      ostr<<"TcpSocketImpl::TcpSocketImpl(int port): 'socket' returned the error code "<<socket_return<<".";
#else
      ostr<<"'socket' returned the error code "<<socket_return<<".";
#endif

#if defined(DEBUG) && defined(FileLogDebugDefined)
      FileLogDebug(ostr.str());
#endif
      throw SocketException(ostr.str());
    }

  bool setsockopt_return=SetReuseAddr();
  if (!setsockopt_return)
    {
      ostringstream ostr;
#if defined(DEBUG)
      ostr<<"TcpSocketImpl::TcpSocketImpl(int port): 'SetReuseAddr' returned the error code "<<setsockopt_return<<".";
#else
      ostr<<"'SetReuseAddr' returned the error code "<<setsockopt_return<<".";
#endif
#if defined(DEBUG) && defined(FileLogDebugDefined)
      FileLogDebug(ostr.str());
#endif
      throw SocketException(ostr.str());
    }

  int bind_return=Bind(port);

  if (bind_return!=0)
    {
      ostringstream ostr;      
#if defined(DEBUG)
      ostr<<"TcpSocketImpl::TcpSocketImpl(int port): 'bind' returned with error code "<<bind_return<<".";
#else
      ostr<<"'bind' returned with error code "<<bind_return<<".";
#endif
#if defined(DEBUG) && defined(FileLogDebugDefined)
      FileLogDebug(ostr.str());
#endif
      throw SocketException(ostr.str());
    }

  int listen_return=Listen();
  if (listen_return!=0)
    {
      ostringstream ostr;      
#if defined(DEBUG)
      ostr<<"TcpSocketImpl::TcpSocketImpl(int port): 'listen' returned with error code "<<listen_return<<".";
#else
      ostr<<"'listen' returned with error code "<<listen_return<<".";
#endif
#if defined(DEBUG) && defined(FileLogDebugDefined)
      FileLogDebug(ostr.str());
#endif
      throw SocketException(ostr.str());
    }

  _boundhost=inet_ntoa(_socket_addr.sin_addr);

}


TcpSocketImpl::~TcpSocketImpl()
{
  Disconnect();
#if defined(linux)
  close(_socket);
  _socket=0;

#elif defined(WIN32)
  closesocket(_socket);
  _socket=0;

  _socket_counter--;

  if (_socket_counter==0)
    {
      ReleaseLib();
    }
#else
#error "TcpSocketImpl::~TcpSocketImpl implementation is empty"
#endif

}

int TcpSocketImpl::Socket()
{
  int socket_return=0;

#if defined(WIN32)
  // If this is the first time we enter this function, then we need to
  // initialize the WSA library first.
  if (_socket_counter==0)
    {
      int lib_return=InitLib();

      if (lib_return!=0)
	{
	  ostringstream ostr;
#if defined(DEBUG)
	  ostr<<"TcpSocketImpl::TcpSocketImpl: 'WSAStartup' returned the error code "<<lib_return<<".";
#else
	  ostr<<"'WSAStartup' returned the error code "<<lib_return<<".";
#endif
#if defined(DEBUG) && defined(FileLogDebugDefined)
	  FileLogDebug(ostr.str());
#endif
	  throw SocketException(ostr.str());
	}
    }
  _socket_counter++;
#endif

  // This class will use only IPv4 for instance.
  _socket=socket(AF_INET,SOCK_STREAM,0);

  if (_socket==INVALID_SOCKET)
    {
#if defined(linux)
      socket_return=errno;
#elif defined(WIN32)
      socket_return=WSAGetLastError();
#else
#error "TcpSocketImpl::Socket: Function won't return the appropriate value in case of error."
#endif
    }
  return socket_return;
}

int TcpSocketImpl::IsReadyForRead(unsigned int ms)
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

int TcpSocketImpl::IsReadyForWrite()
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

int TcpSocketImpl::Send(const char *buf, int bufsize)
{
  int status;

#if defined(DEBUG) && defined(FileLogDebugDefined)
  ostringstream ostr2;      
  ostr2<<"TcpSocketImpl::Send: sending request of the "<<bufsize<<" following bytes:\n";
  for (int i=0; i<bufsize; i++)
    {
      int hexvalue;
      char *p=(char *) &hexvalue;
      hexvalue=Byte2Hex(buf[i]);
      ostr2<<p[0]<<p[1]<<" ";
    }
  FileLogDebug(ostr2.str());
#endif


#if defined(linux)
  status=::send(_socket, buf, bufsize,MSG_EOR|MSG_NOSIGNAL);

  if (status==SOCKET_ERROR)
    {
      status=errno;
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Send: 'send' returned the error code "<<status<<".";
      FileLogDebug(ostr.str());
#endif
      if (status>0) status=-status;
    }
#elif defined(WIN32)
  status=::send(_socket, buf, bufsize,0);

  if (status==SOCKET_ERROR) 
    {
      status=WSAGetLastError();
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Send: 'send' returned the error code "<<status<<".";
      FileLogDebug(ostr.str());
#endif

      if (status>0) status=-status;
    }
#else
#error "TcpSocketImpl::Send: Implementation of this function is empty."
#endif
  return status;
}

int TcpSocketImpl::Recv(string &s)
{
  char tmpbuf[MAXRECV];

  int status=::recv(_socket,tmpbuf,MAXRECV,0);

  s.erase();

  if (status==SOCKET_ERROR)
    {
#if defined(linux)
      status=errno;

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Recv(const &string): 'recv' received error "<<status<<".\n";
      FileLogDebug(ostr.str());
#endif

      if (status>0) status=-status;
#elif defined(WIN32)
      status=WSAGetLastError();

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Recv(const &string): 'recv' received error "<<status<<".\n";
      FileLogDebug(ostr.str());
#endif
      if (status>0) status=-status;
#else
#error "TcpSocketImpl::Recv: Cannot return the appropriate error code."
#endif
      return status;
    }
  else if ( status == 0 )
    {
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Recv(const &string): 'recv' received 0 bytes.\n";
      FileLogDebug(ostr.str());
#endif      
      return 0;
    }
  else
    {
#if defined(DEBUG) && defined(FileLogDebugDefined)
  ostringstream ostr;      
  ostr<<"TcpSocketImpl::Recv(const &string): 'recv' received "<<status<<" bytes:\n";
  for (int i=0; i<status; i++)
    {
      int hexvalue;
      char *p=(char *) &hexvalue;
      hexvalue=Byte2Hex(tmpbuf[i]);
      ostr<<p[0]<<p[1]<<" ";
    }
  FileLogDebug(ostr.str());
#endif

      s.append(tmpbuf,status);
      return status;
    }
}

int TcpSocketImpl::Recv(char *buf, int max_buf_size, int *received_size)
{
  int status;
  status= ::recv(_socket,buf, max_buf_size,0);

  if (status==SOCKET_ERROR)
    {
#if defined(linux)
      status=errno;

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Recv(char *buf, int, int *): 'recv' received error "<<status<<".\n";
      FileLogDebug(ostr.str());
#endif

      if (status>0) status=-status;
#elif defined(WIN32)
      status=WSAGetLastError();

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Recv(char *buf, int, int *): 'recv' received error "<<status<<".\n";
      FileLogDebug(ostr.str());
#endif

      if (status>0) status=-status;
#else
#error "TcpSocketImpl::Recv: Cannot return the appropriate error code."
#endif
      *received_size=0;
      return status;
    }
  else if (status==0)
    {
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Recv(char *buf, int, int *): 'recv' received 0 bytes.\n";
      FileLogDebug(ostr.str());
#endif      
      *received_size = 0;
      return 0;
    }
  else
    {
#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Recv(char *buf, int, int *): 'recv' received "<<status<<" bytes:\n";
      for (int i=0; i<status; i++)
	{
	  int hexvalue;
	  char *p=(char *) &hexvalue;
	  hexvalue=Byte2Hex(buf[i]);
	  ostr<<p[0]<<p[1]<<" ";
	}
      FileLogDebug(ostr.str());
#endif

      *received_size=status;
      return status;
    }
}

int TcpSocketImpl::Disconnect()
{
  int res=0;

  if (!_connected) return 0;

  _connected=false;

#if defined(linux)
  res=shutdown(_socket,SHUT_RDWR);
  if (res==SOCKET_ERROR)
    {
      res=errno;
    }
#elif defined(WIN32)
  res=shutdown(_socket,SD_BOTH);
  if (res==SOCKET_ERROR)
    {
      res=WSAGetLastError();
    }

#if defined(DEBUG) && defined(FileLogDebugDefined)
  ostringstream ostr;      
  ostr<<"TcpSocketImpl::Disconnect(): 'shutdown' returned "<<res<<".";
  FileLogDebug(ostr.str());
#endif      
  
#else
#error "Implementation of TcpSocketImpl::Disconnect is empty"
#endif

  return res;
}


int TcpSocketImpl::Connect(const string &host, int port)
{
  _socket_addr.sin_family=AF_INET;
  _socket_addr.sin_port=htons(port);

  tInetAddr inetaddr=inet_addr(host.c_str());

  if (inetaddr==INADDR_NONE)
    {
      // host isn't an IP address (dot notation) so we have to resolve
      // the name with DNS.

      struct hostent* phost = gethostbyname(host.c_str());

      if (phost != 0)
	{
	  inetaddr = *((tInetAddr *)phost->h_addr_list[0]);
        }

    }

  memcpy((void *)&_socket_addr.sin_addr.s_addr, (void *)&inetaddr, sizeof(inetaddr));

  int status=::connect(_socket,(sockaddr *)&_socket_addr,sizeof(_socket_addr));

  if (status==SOCKET_ERROR)
    {
#if defined(linux)
      status=errno;
#elif defined(WIN32)
      status=WSAGetLastError();
#else
#error "TcpClientSocketImpl::Connect won't return a good value in case of runtime error."
#endif

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Connect(): 'connect' failed with error "<<status<<".";
      FileLogDebug(ostr.str());
#endif      
    }
  else
    {
      _connected=true;
      char _tmpbuf[100];
      
      strcpy(&_tmpbuf[0],inet_ntoa(_socket_addr.sin_addr));
      _boundhost=_tmpbuf;
    }
  return (status);
}

int TcpSocketImpl::Bind(int port)
{
  _socket_addr.sin_family = AF_INET;
  _socket_addr.sin_addr.s_addr = INADDR_ANY;
  _socket_addr.sin_port=htons(port);

  int bind_return = ::bind(_socket,
			   (struct sockaddr *) &_socket_addr,
			   sizeof (_socket_addr) );

  if (bind_return==SOCKET_ERROR)
    {
#if defined(linux)
      bind_return=errno;
#elif defined(WIN32)
      bind_return=WSAGetLastError();
#else
#error "TcpSocketImpl::Bind(int port) won't return a good value."
#endif

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Bind(): 'bind' failed with error "<<bind_return<<" on port "<<port<<".";
      FileLogDebug(ostr.str());
#endif      


    }

  return bind_return;
}


int TcpSocketImpl::Listen()
{
  int listen_return=::listen(_socket,MAXCONNECTIONS);

  if (listen_return==SOCKET_ERROR)
    {
#if defined(linux)
      listen_return=errno;
#elif defined(WIN32)
      listen_return=WSAGetLastError();
#else
#error "TcpSocketImpl::Listen() won't return a good value."
#endif

#if defined(DEBUG) && defined(FileLogDebugDefined)
      ostringstream ostr;      
      ostr<<"TcpSocketImpl::Listen(): 'listen' failed with error "<<listen_return<<".";
      FileLogDebug(ostr.str());
#endif      

    }

  return listen_return;
}


TcpSocket *TcpSocketImpl::Accept()
{
  int addr_length = sizeof ( _socket_addr );
  sockaddr_in socket_addr;
  SOCKET socket;

  socket=::accept(_socket,
		  (sockaddr*) &socket_addr,
#if defined(__GNUC__)
		  (socklen_t *) &addr_length );
#else
		   &addr_length );
#endif

  if (socket==INVALID_SOCKET)
    {
      int accept_return;
#if defined(linux)
      accept_return=errno;
#elif defined(WIN32)
      accept_return=WSAGetLastError();
#else
#error "TcpSocketImpl::Accept() won't be able to return an appropriate error code in Exception."
#endif
      ostringstream ostr;
#if defined(DEBUG)
      ostr<<"TcpSocketImpl::Accept returned the error code "<<accept_return<<".";
#else
      ostr<<"'accept' returned the error code "<<accept_return<<".";
#endif
#if defined(DEBUG) && defined(FileLogDebugDefined)
      FileLogDebug(ostr.str());
#endif      
      throw SocketException(ostr.str());
    }

  return new TcpSocket(new TcpSocketImpl(socket,socket_addr,true));

}


bool TcpSocketImpl::IsConnected() const
{
  return _connected;
}


const string &TcpSocketImpl::PeerHostAddress() const
{  
  return _boundhost;
}
unsigned short TcpSocketImpl::PeerHostPort() const
{  
  return (unsigned short)ntohs(_socket_addr.sin_port);
}

string TcpSocketImpl::PeerLocalAddress() const
{  
	struct sockaddr localaddr={0};
	int len = sizeof(sockaddr);

	int res = getsockname ( _socket, &localaddr, &len );

	string locala = string(inet_ntoa (((struct sockaddr_in *)&localaddr)->sin_addr ));

	if ( res == SOCKET_ERROR )
		return "";
	else
		return string(inet_ntoa (((struct sockaddr_in *)&localaddr)->sin_addr ));
}
unsigned short TcpSocketImpl::PeerLocalPort() const
{
	struct sockaddr localaddr={0};
	int len = sizeof(sockaddr);

	int res = getsockname ( _socket, &localaddr, &len ); 

	if ( res == SOCKET_ERROR ) 
		return 0;
	else
		return ntohs (((struct sockaddr_in *)&localaddr)->sin_port );
}
bool TcpSocketImpl::SetBufRcvSize(const tUInt32 newsize)
{
  int res;

  res=setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (const char *)&newsize, sizeof(tUInt32));

  return (res==0);
}

bool TcpSocketImpl::SetBufSndSize(const tUInt32 newsize)
{
  int res;

  res=setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (const char *)&newsize, sizeof(tUInt32));

  return (res==0);
}

bool TcpSocketImpl::SetKeepAlive()
{
  int res;
  int on;
  on=1;
  res=setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, (const char *)&on, sizeof(int));

  return (res==0);
}

bool TcpSocketImpl::ClearKeepAlive()
{
  int res;
  int off;

  off=0;
  res=setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, (const char *)&off, sizeof(int));

  return (res==0);
}

bool TcpSocketImpl::SetReuseAddr()
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

bool TcpSocketImpl::ClearReuseAddr()
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

SOCKET TcpSocketImpl::GetHandle()
{
  return _socket;
}
