#ifndef __SCONN_HANDLER_MANAGER_HXX__
#define __SCONN_HANDLER_MANAGER_HXX__

#include <map>
#include <vector>
#include "mytypes.h"
#include "mythread.hxx"

class FileLog;
class SConnHandler;

class SConnManager : public Thread
{
public:
  ////////////////////////////////////////TYPEDEFS//////////////////////////////////////
  /////////map is for handler to register itself////////////////////////////////////////
  typedef map<tUInt32,SConnHandler*>	tIdSConnMap;
  typedef tIdSConnMap::const_iterator	tIdSConnMapConstIter;
  typedef tIdSConnMap::iterator			tIdSConnIter;
  typedef pair<tUInt32,SConnHandler*>	tIdSConnPair;

  ////////vector is for manager to manage handler///////////////////////////////////////
  typedef vector<SConnHandler*> tSConnHandlerVec;
  typedef tSConnHandlerVec::iterator tSConnHandlerIter;
  typedef tSConnHandlerVec::const_iterator tSConnHandlerConstIter;
  //////////////////////////////////////////////////////////////////////////////////////
  
  SConnManager(tUInt32 sport,FileLog *filelog=NULL);

  virtual ~SConnManager();

  void Shutdown();

  tBool FindMostLeisureSlave(tUInt32& id,string& slave_host,tUInt32& slave_port) const;

protected:
	//
  virtual void Execute(Thread::Arg arg);

private:
	//
  SConnManager(const SConnManager &model);

  /**
   * Clean the connneciton handlers that are not active any longer
   */
  void CleanShutdownConnections();

  /**
   * Shutdown all connection handlers and join for all
   * called when shutdown requested.
   */
  void ShutdownAllConnections();


  /**
   * Containing all connection to Slave Servers.
   */
  tIdSConnMap _id_sconn_map;
  tSConnHandlerVec _handler_vec;

  tUInt32	_sport;

  tBool _is_shutdown_requested;

  FileLog*	_filelog;
};

#endif // __SCONN_HANDLER_MANAGER_HXX__
