#ifndef __AXAUDIOETEXCEPTION_HXX__
#define __AXAUDIOETEXCEPTION_HXX__

#include <string>
#include <iostream>
#include "axaudio.hxx"

using namespace std;

class EXPORT_AXAUDIO AXAudioException
{
 public:
  AXAudioException(const string &msg);
  ~AXAudioException();

  const string &Description() const;

 private:

  string _msg;

};

/// \brief Operator allowing to output exception description directly into a stream.
ostream &operator<<(ostream &os, const AXAudioException &e);


#endif // __AXAUDIOETEXCEPTION_HXX__
