//#include "scrambled_symbols.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(linux)
#include <unistd.h>
#endif

#include "filebuffer.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include <sstream>

using namespace std;

tUInt32 FileBuffer::_session_file_open_error_counter=0;
tUInt32 FileBuffer::_session_file_read_error_counter=0;
tUInt32 FileBuffer::_session_file_create_error_counter=0;
tUInt32 FileBuffer::_session_file_write_error_counter=0;

void FileBuffer::ResetCounters()
{
  _session_file_open_error_counter=0;
  _session_file_read_error_counter=0;
  _session_file_create_error_counter=0;
  _session_file_write_error_counter=0;
}

bool FileBuffer::Read(const string &filename, tByte *buffer, tUInt32 maxsize, tUInt32 *buffersize, FileLog *_filelog)
{
  FILE *fin;
  int res;

  *buffersize=0;

  fin=fopen(filename.c_str(), "rb");
  
  if (fin==0)
    {
      if (_session_file_open_error_counter==0)
	{
#if defined(DEBUG)
	  LOGWARNING("ReadBuffer: Failed to open file '"<<filename<<"'.");
#else
	  LOGWARNING("Missing file(s) detected. (Ignore if previous download didn't completed).");
#endif
	  _session_file_open_error_counter++;
	}
      return false;
    }
    
  while(!feof(fin))
    {
      res=fread(buffer + (*buffersize), 1, maxsize-(*buffersize), fin);
      
      if (res<=0)
	{
	  bool status;
	  if (ferror(fin))
	    {
	      if (_session_file_read_error_counter==0)
		{
#if defined(DEBUG)
		  LOGERROR("ReadBuffer: Error while reading in '"<<filename<<"'.");
#else
		  LOGERROR("File system problem detected while reading. Check your hard disk health!");
#endif
		}
	      _session_file_read_error_counter++;
	      status=false;
	    }
	  else
	    {
	      status=true;
	    }
	  fclose(fin);
	  return status;
	}
      *buffersize+=res;
    } // while(!feof(fin))
  
  fclose(fin);
  return true;
}

bool FileBuffer::Write(const string &filename, const tByte *buffer, tUInt32 buffersize, FileLog *_filelog)
{
  FILE * fout;
  tUInt32 bytecounter;
  int res;

  // Writing buffer to file.
  fout=fopen(filename.c_str(), "wb");
  if (fout==0)
    {
      if (_session_file_create_error_counter==0)
	{
#if defined(DEBUG)
	  LOGERROR("WriteBuffer: Failed to create file '"<<filename<<"'.");
#else
	  LOGERROR("Failed to create file into repository. Please check repository permission.");
#endif
	  _session_file_create_error_counter++;
	}
      return false;
    }
      
  bytecounter=0;
  while (bytecounter<buffersize)
    {
      res=fwrite(buffer+bytecounter,1, buffersize-bytecounter,fout);
	  
      if (res<=0)
	{
	  if (ferror(fout))
	    {
	      if (_session_file_write_error_counter==0)
		{
#if defined(DEBUG)
		  LOGERROR("WriteBuffer: Error while writing in '"<<filename<<"'  when "<<bytecounter<<" bytes written so far.");
#else
		  LOGERROR("Failed to write data into file. Whether harddisk is full of file system is corrupted. Please check!");
#endif
		  _session_file_write_error_counter++;
		}
	      fclose(fout);
	      return false;
	    }
	  else
	    {
	      break;
	    }
	}

      bytecounter+=res;

   } // while (bytecounter<buffersize)
  fclose(fout);
  return true;
}


FileBuffer::tFileExistenceStatus FileBuffer::Exists(const string &filename)
{
  struct stat statfile;
  int res;

  res=stat(filename.c_str(), &statfile);

  if (res==0)
    {
      return eEXIST;
    }
  
  if (errno!=ENOENT)
    {
      switch(errno)
	{
	case ENOTDIR:
	  {
	    return eBADPATHERROR;
	  }
	  break;
	case EACCES:
	  {
	    return eNOPERMISSIONERROR;
	  }
	  break;
	case ENAMETOOLONG:
	  {
	    return eNAMETOOLONGERROR;
	  }
	  break;
	default:
	  {
	    return eERROR;
	  }
	  break;
	}
    }
  
  return eDONTEXIST;
      
}
