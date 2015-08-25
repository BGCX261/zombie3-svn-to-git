#ifndef __TCPCLIENTSOCKET_HXX__
#define __TCPCLIENTSOCKET_HXX__

#include "tcpsocket.hxx"

using namespace std;

// The class TcpSocketImpl will hold the real and effective implementation of
// the class below. We will use delegation in order to hide implementation 
// details (in order to make real Object Oriented else we can go do find 
// another job or seek a job at Microsoft - just kidding for the last one).
class TcpSocketImpl;


/// \brief A class to use a client TCP socket (connected mode).
///
/// A TCP socket client is used to connect to a TCP socket server
/// on a given port.
class EXPORT_MYNET TcpClientSocket : public TcpSocket
{
 public:
  /// \brief Constructor.
  TcpClientSocket();
  /// \brief Destructor.
  virtual ~TcpClientSocket();

  virtual int Connect(const string &host,int port);

 private:
  // Copy constructor is private to FORCE the user to instantiate 
  // a new object with 'new' instead of static allocation.
  // This is because we cannot copy this object so easily 
  // mainly because of internal objects.
  TcpClientSocket(const TcpClientSocket &model);
};


#endif // __TCPCLIENTSOCKET_HXX__
