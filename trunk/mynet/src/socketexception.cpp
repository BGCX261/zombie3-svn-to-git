#include "socketexception.hxx"

SocketException::SocketException(const string &msg)
  : _msg(msg)
{
}

SocketException::~SocketException()
{
}

const string &SocketException::Description() const 
{
  return _msg;
}

ostream &operator<<(ostream &os, const SocketException &s)
{
  return(os<<s.Description());
}
