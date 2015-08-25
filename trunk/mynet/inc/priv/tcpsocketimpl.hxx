#ifndef _TCPSOCKETIMPL_HXX_
#define _TCPSOCKETIMPL_HXX_

#if defined(linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// The following macros/types are defined in order to ease the 
// compatibility of this class on both Windows and Linux.
#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)
typedef int SOCKET;

#include <errno.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

#elif defined(WIN32)

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#include <winsock2.h>
#include <ws2tcpip.h>

#else

#error "TcpSocket class is unsupported on this platform."

#endif

#include <string>
#include <iostream>

using namespace std;

// This class will contain the real implementation of the TcpSocket,
// TcpClientSocket and TcpServerSocket. All these three classes 
// MUST share the same TcpSocketImpl definition.
class TcpSocket;
class TcpSocketImpl
{
 public:
#if defined(linux)
typedef in_addr_t tInetAddr;
#elif defined(WIN32)
typedef unsigned long tInetAddr;
#else
#endif

  TcpSocketImpl();
  TcpSocketImpl(int port);
  virtual ~TcpSocketImpl();

  // For all kinds of Socket
  virtual int Send(const char *buf,int bufsize);
  virtual int Recv(string& data);
  virtual int Recv(char *buf, int max_buf_size,int *received_size);
  virtual int IsReadyForRead(unsigned int ms);
  virtual int IsReadyForWrite();

  // For Client Socket
  virtual int Connect(const string &host, int port);
  virtual int Disconnect();
  virtual bool IsConnected() const;
  virtual const string &PeerHostAddress() const;
  virtual unsigned short PeerHostPort() const;
  virtual string PeerLocalAddress() const;
  virtual unsigned short PeerLocalPort() const;

  // For Server initialization
  virtual TcpSocket *Accept();



  virtual bool SetBufRcvSize(const tUInt32 newsize);
  virtual bool SetBufSndSize(const tUInt32 newsize);

  virtual bool SetKeepAlive();
  virtual bool ClearKeepAlive();

  virtual bool SetReuseAddr();
  virtual bool ClearReuseAddr();

  virtual SOCKET GetHandle();

 protected:
  // For Server Socket
  virtual int Bind(int port);
  virtual int Listen();
  virtual int Socket();

  // Following constructor to allow Accept function to 
  // create a new socket.
  TcpSocketImpl(SOCKET socket, sockaddr_in &socket_addr, bool connected);

  SOCKET _socket;
  sockaddr_in _socket_addr;
  bool _connected;
  string _boundhost;
};

#endif // _TCPSOCKETIMPL_HXX_
