#include "tcpsocket.hxx"
#include "priv/tcpsocketimpl.hxx"
#include <assert.h>
#include <iostream>

using namespace std;

TcpSocket::TcpSocket()
 : _tcpsocketimpl()
{
  _tcpsocketimpl=new TcpSocketImpl();
}

TcpSocket::TcpSocket(const TcpSocket &model)
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"TcpSocket::TcpSocket(const TcpSocket &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

TcpSocket::TcpSocket(int port)
 : _tcpsocketimpl()
{
  _tcpsocketimpl=new TcpSocketImpl(port);  
}

TcpSocket::TcpSocket(TcpSocketImpl *impl)
 : _tcpsocketimpl(impl)
{
}

TcpSocket::~TcpSocket()
{
  if (_tcpsocketimpl) delete _tcpsocketimpl;
  _tcpsocketimpl=0;
}

int TcpSocket::Send(const string &s)
{
  return _tcpsocketimpl->Send(s.c_str(), s.size());
}

int TcpSocket::Send(const char *buf, int bufsize)
{
  return _tcpsocketimpl->Send(buf,bufsize);
}

int TcpSocket::Recv(string &s)
{
  return _tcpsocketimpl->Recv(s);
}

int TcpSocket::Recv(char *buf, int max_buf_size, int *received_size)
{
  return _tcpsocketimpl->Recv(buf,max_buf_size, received_size);
}

int TcpSocket::IsReadyForRead(unsigned int ms)
{
  return _tcpsocketimpl->IsReadyForRead(ms);
}

int TcpSocket::IsReadyForWrite()
{
  return _tcpsocketimpl->IsReadyForWrite();
}

int TcpSocket::Disconnect()
{
  return _tcpsocketimpl->Disconnect();
}

bool TcpSocket::IsConnected() const
{
  return _tcpsocketimpl->IsConnected();
}

const string &TcpSocket::PeerHostAddress() const
{
  return _tcpsocketimpl->PeerHostAddress();
}
unsigned short TcpSocket::PeerHostPort() const
{
  return _tcpsocketimpl->PeerHostPort();
}
string TcpSocket::PeerLocalAddress() const
{
  return _tcpsocketimpl->PeerLocalAddress();
}
unsigned short TcpSocket::PeerLocalPort() const
{
  return _tcpsocketimpl->PeerLocalPort();
}

bool TcpSocket::SetBufRcvSize(const tUInt32 newsize)
{
  return _tcpsocketimpl->SetBufRcvSize(newsize);
}

bool TcpSocket::SetBufSndSize(const tUInt32 newsize)
{
  return _tcpsocketimpl->SetBufSndSize(newsize);
}

bool TcpSocket::SetKeepAlive()
{
  return _tcpsocketimpl->SetKeepAlive();
}

bool TcpSocket::ClearKeepAlive()
{
  return _tcpsocketimpl->ClearKeepAlive();
}


bool TcpSocket::SetReuseAddr()
{
  return _tcpsocketimpl->SetReuseAddr();
}

bool TcpSocket::ClearReuseAddr()
{
  return _tcpsocketimpl->ClearReuseAddr();
}

SOCKET TcpSocket::GetHandle()
{
  return _tcpsocketimpl->GetHandle();
}
