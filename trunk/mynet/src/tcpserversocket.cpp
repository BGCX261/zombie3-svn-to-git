#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>

#include "tcpserversocket.hxx"
#include "priv/tcpsocketimpl.hxx"

using namespace std;

TcpServerSocket::TcpServerSocket(int port) 
  : TcpSocket(port)
{
}

TcpServerSocket::~TcpServerSocket()
{
}

TcpSocket *TcpServerSocket::Accept()
{
  return _tcpsocketimpl->Accept();
}

TcpServerSocket::TcpServerSocket(const TcpServerSocket &model)
  : TcpSocket()
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"TcpServerSocket::TcpServerSocket(const TcpServerSocket &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}
