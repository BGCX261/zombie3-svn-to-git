#include "axaudioexception.hxx"

AXAudioException::AXAudioException(const string &msg)
  : _msg(msg)
{
}

AXAudioException::~AXAudioException()
{
}

const string &AXAudioException::Description() const 
{
  return _msg;
}

ostream &operator<<(ostream &os, const AXAudioException &s)
{
  return(os<<s.Description());
}
