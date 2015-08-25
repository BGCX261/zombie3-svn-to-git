#ifndef __SOCKETEXCEPTION_HXX__
#define __SOCKETEXCEPTION_HXX__

#include <string>
#include <iostream>
//#include "mydll.h"
#include "tcpsocket.hxx"

using namespace std;

class EXPORT_MYNET SocketException
{
 public:
  SocketException(const string &msg);
  ~SocketException();

  const string &Description() const;

 private:

  string _msg;

};

/// \brief Operator allowing to output exception description directly into a stream.
ostream &operator<<(ostream &os, const SocketException &e);


#endif // __SOCKETEXCEPTION_HXX__
