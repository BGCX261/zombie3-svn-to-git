#ifndef __TCPSERVERSOCKET_HXX__
#define __TCPSERVERSOCKET_HXX__

#include "tcpsocket.hxx"

using namespace std;


// The class TcpSocketImpl will hold the real and effective implementation of
// the class below. We will use delegation in order to hide implementation 
// details (in order to make real Object Oriented else we can go do find 
// another job or seek a job at Microsoft - just kidding for the last one).
class TcpSocketImpl;


/// \brief A class to use as Server TCP socket (connected mode).
///
/// A TCP socket server is used to be connected by a TCP socket 
/// client on a given port in order to exchange data.
/// The socket binds and listens on the given port number as soon 
/// as it has been created. So all the usage of this class is to
/// obtain new incoming connections with Accept.
class EXPORT_MYNET TcpServerSocket : public TcpSocket
{
 public:
  /// \brief Constructor.
  TcpServerSocket(int port);
  /// \brief Destructor.
  virtual ~TcpServerSocket();

  virtual TcpSocket *Accept();

 protected:

 private:
  // Copy constructor is private to FORCE the user to instantiate 
  // a new object with 'new' instead of static allocation.
  // This is because we cannot copy this object so easily 
  // mainly because of internal objects.
  TcpServerSocket(const TcpServerSocket &model);
};


#endif // __TCPSERVERSOCKET_HXX__
