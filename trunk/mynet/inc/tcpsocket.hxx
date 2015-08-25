#ifndef __TCPSOCKET_HXX__
#define __TCPSOCKET_HXX__

//#include "mydll.h"
#if defined(WIN32)

#ifdef EXPORT
#undef EXPORT
#endif

#if defined(MYNET_EXPORTS)
/* DLL export */
#ifdef EXPORT_MYNET
#undef EXPORT_MYNET
#endif
#define EXPORT_MYNET __declspec(dllexport)
#else
/* EXE import */
#define EXPORT_MYNET __declspec(dllimport)
//#define EXPORT
#endif

#ifdef WIN32
#pragma comment(lib,"Ws2_32")
#endif

#else
#define EXPORT_MYNET 
#endif

#include "mytypes.h"
#include <string>

using namespace std;

// The class TcpSocketImpl will hold the real and effective implementation of
// the class below. We will use delegation in order to hide implementation 
// details (in order to make real Object Oriented else we can go do find 
// another job or seek a job at Microsoft - just kidding for the last one).
class TcpSocketImpl;

#if defined(linux)
typedef int SOCKET;
#elif defined(WIN32)
#include <winsock2.h>
#else
#error "Unsupported OS"
#endif

/// \brief A class to handle TCP/IP socket (connected mode).
///
/// This class is just an Object Oriented abstraction of the standard library.
///
class EXPORT_MYNET TcpSocket
{
 public:
  /// \brief Constructor.
  TcpSocket();

  /// \brief Destructor.
  virtual ~TcpSocket();

  // Data Transmission & Reception
  virtual int Send(const string &data);
  virtual int Send(const char *buf,int bufsize);

  virtual int Recv(string &data);
  virtual int Recv(char *buf, int max_buf_size,int *received_size);

  /// \brief Test whether a socket can accept to read data without blocking
  /// In case of error, a negative value is returned containing the (negative)
  /// error code. If 0 is returned then no event occured. If a strictly 
  /// positive value is returned, then data is available for read.
  virtual int IsReadyForRead(unsigned int ms=0);

  /// \brief Test whether a socket can accept to send data without blocking
  /// In case of error, a negative value is returned containing the (negative)
  /// error code. If 0 is returned then no event occured. If a strictly 
  /// positive value is returned, then socket is available for writing.
  virtual int IsReadyForWrite();

  virtual int Disconnect();

  virtual bool IsConnected() const;

  virtual const string &PeerHostAddress() const;
  virtual unsigned short PeerHostPort() const;

  virtual string PeerLocalAddress() const;
  virtual unsigned short PeerLocalPort() const;

  virtual bool SetBufRcvSize(const tUInt32 newsize);
  virtual bool SetBufSndSize(const tUInt32 newsize);

  virtual bool SetKeepAlive();
  virtual bool ClearKeepAlive();

  virtual bool SetReuseAddr();
  virtual bool ClearReuseAddr();

  virtual SOCKET GetHandle();

 protected:
  friend class TcpSocketImpl;
  TcpSocket(TcpSocketImpl *impl);
  TcpSocket(int port);
  TcpSocketImpl *_tcpsocketimpl;

 private:
  // Copy constructor is private to FORCE the user to instantiate 
  // a new object with 'new' instead of static allocation.
  // This is because we cannot copy this object so easily 
  // mainly because of internal objects.
  TcpSocket(const TcpSocket &model);
};


#endif // __TCPSOCKET_HXX__
