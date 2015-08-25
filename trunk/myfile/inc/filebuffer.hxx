#ifndef _FILEBUFFER_HXX_
#define _FILEBUFFER_HXX_

#include "myfilelog.hxx"

#include "mytypes.h"


class FileLog;

class EXPORT_MYFILE FileBuffer
{
 public:
  typedef enum
  {
    eUNKNOWN,
    eEXIST,
    eDONTEXIST,
    eERROR,
    eBADPATHERROR,
    eNOPERMISSIONERROR,
    eNAMETOOLONGERROR
  } tFileExistenceStatus;

  static bool Read(const string &filename, tByte *buffer, tUInt32 maxsize, tUInt32 *buffersize, FileLog *_filelog);
  static bool Write(const string &filename, const tByte *buffer, tUInt32 buffersize, FileLog *_filelog);
  static tFileExistenceStatus Exists(const string &filename);

  static void ResetCounters();

private:
  static tUInt32 _session_file_open_error_counter;
  static tUInt32 _session_file_read_error_counter;
  static tUInt32 _session_file_create_error_counter;
  static tUInt32 _session_file_write_error_counter;

};

#endif // _FILEBUFFER_HXX_

