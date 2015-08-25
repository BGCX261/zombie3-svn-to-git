#include "udpsocketexception.hxx"

UdpSocketException::UdpSocketException(const string &msg)
  : _msg(msg)
{
}

UdpSocketException::~UdpSocketException()
{
}

const string &UdpSocketException::Description() const 
{
  return _msg;
}

ostream &operator<<(ostream &os, const UdpSocketException &s)
{
  return(os<<s.Description());
}
