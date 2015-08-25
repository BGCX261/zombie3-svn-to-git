#ifndef __CCONN_MANAGER_HXX__
#define __CCONN_MANAGER_HXX__

#include <vector>
#include "mytypes.h"
#include "mythread.hxx"

class SConnManager;
class CConnHandler;
class FileLog;

class CConnManager : public Thread
{
public:
  ////////////////////////////////////////TYPEDEFS//////////////////////////////////////////
  typedef vector<CConnHandler*> tCConnHandlerVec;
  typedef tCConnHandlerVec::iterator tCConnHandlerIter;
  typedef tCConnHandlerVec::const_iterator tCConnHandlerConstIter;
  //////////////////////////////////////////////////////////////////////////////////////////
  
  CConnManager(
	  SConnManager* sconnmanager,
	  tUInt32 permanent_handler_num,
	  tUInt32 handlernummax,
	  tUInt32 connmaxofhandler,
	  tUInt32 cport,
	  string dbhost,
	  tUInt32 dbport,
	  string dbname,
	  string dblogin,
	  string dbpassword,
	  FileLog *filelog=NULL);

  virtual ~CConnManager();

  void Shutdown();

  tBool FindMostLeisureSlave(tUInt32& id,string& slave_ip,tUInt32& slave_port) const;

protected:
	//
  virtual void Execute(Thread::Arg arg);

private:
	//
  CConnManager(const CConnManager &model);

  /**
   * Clean the connneciton handlers that are not active any longer
   */
  void CleanShutdownConnections();

  /**
   * Shutdown all connection handlers and join for all
   * called when shutdown requested.
   */
  void ShutdownAllConnections();

  CConnHandler* FindMostLeisureHandler(void) const;

  /**
   * Containing all connection hanlers' handle(pointer)
   */
  tCConnHandlerVec _handler_vec;

  SConnManager* _sconnmanager;

  tUInt32	_permanent_handler_num;
  tUInt32	_handler_max;
  tUInt32	_connmaxofhandler;

  tUInt32	_listen_port;

  string	_dbhost;
  tUInt32	_dbport;
  string	_dbname;
  string	_dblogin;
  string	_dbpassword;

  tBool		_is_shutdown_requested;

  FileLog*	_filelog;
};

#endif // __CCONN_MANAGER_HXX__
