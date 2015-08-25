#ifndef __UDPSOCKETEXCEPTION_HXX__
#define __UDPSOCKETEXCEPTION_HXX__

#include <string>
#include <iostream>
#include "udpsocket.hxx"

//#include "mydll.h"

using namespace std;

class EXPORT_MYUDPNET UdpSocketException
{
 public:
  UdpSocketException(const string &msg);
  ~UdpSocketException();

  const string &Description() const;

 private:

  string _msg;

};

/// \brief Operator allowing to output exception description directly into a stream.
ostream &operator<<(ostream &os, const UdpSocketException &e);


#endif // __UDPSOCKETEXCEPTION_HXX__
