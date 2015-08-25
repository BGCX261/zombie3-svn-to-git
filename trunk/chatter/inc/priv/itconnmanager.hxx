#ifndef __ITCONN_MANAGER_HXX__
#define __ITCONN_MANAGER_HXX__

#include <vector>
#include <map>
#include "mytypes.h"
#include "mydll.h"
#include "mythread.hxx"
#include "priv/groupmgr.hxx"
#include "socketexception.hxx"
#include "tcpserversocket.hxx"

class FileLog;
class TConn;
class Chatter;

using namespace std;

class ITConnManager : public Thread
{
public:
  ////////////////////////////////////////TYPEDEFS//////////////////////////////	
  //typedef map<tUInt32,tMemInfo*> tMemInfoMap;
  //typedef tMemInfoMap::iterator	tMemInfoMapIter;
  //typedef pair<tUInt32,tMemInfo*> tIntMemInfoPair;

	//
  typedef vector<TConn*> tITConnVec;
  
  ITConnManager(tUInt32 listen_port,FileLog *filelog=NULL);

  virtual ~ITConnManager();

  void Shutdown();

protected:
	//
  virtual void Execute(Thread::Arg arg);

private:
	//
  ITConnManager(const ITConnManager &model);

  /**
   * Clean the connneciton handlers that are not active any longer
   */
  void CleanShutdownConnections();

  /**
   * Shutdown all connection handlers and join for all called when shutdown requested.
   */
  void ShutdownAllConnections();

  /**
   * Containing all connection hanlers' handle(pointer)
   */
  tITConnVec _connection_vec;

  tUInt32 _listen_port;

  tBool _is_shutdown_requested;

  FileLog *_filelog;
};

#endif // __ITCONN_MANAGER_HXX__
