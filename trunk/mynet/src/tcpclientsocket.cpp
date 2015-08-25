#include "tcpclientsocket.hxx"
#include "priv/tcpsocketimpl.hxx"
#include <assert.h>
#include <iostream>

using namespace std;

TcpClientSocket::TcpClientSocket()
  : TcpSocket()
{
}

TcpClientSocket::TcpClientSocket(const TcpClientSocket &model)
  : TcpSocket()
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"TcpClientSocket::TcpClientSocket(const TcpClientSocket &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

TcpClientSocket::~TcpClientSocket()
{
}

int TcpClientSocket::Connect(const string &host, int port)
{
  return _tcpsocketimpl->Connect(host,port);
}

