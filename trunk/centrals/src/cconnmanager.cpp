#include "mytimer.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "socketexception.hxx"
#include "tcpserversocket.hxx"
#include "priv/sconnmanager.hxx"
#include "priv/cconnhandler.hxx"
#include "priv/cconnmanager.hxx"

CConnManager::CConnManager(
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
	  FileLog *filelog)
  : Thread(),
    _handler_vec(),
	_sconnmanager(sconnmanager),
	_permanent_handler_num(permanent_handler_num),
	_handler_max(handlernummax),
	_connmaxofhandler(connmaxofhandler),
    _listen_port(cport),
	_dbhost(dbhost),
	_dbport(dbport),
	_dbname(dbname),
	_dblogin(dblogin),
	_dbpassword(dbpassword),
    _is_shutdown_requested(FALSE),
    _filelog(filelog)
{
}
CConnManager::~CConnManager()
{
}
void CConnManager::Shutdown()
{
  _is_shutdown_requested=true;
}
tBool CConnManager::FindMostLeisureSlave(tUInt32& id,string& slave_ip,tUInt32& slave_port) const
{
	/* slave doesn't use this method */
	if (!_sconnmanager)
		return FALSE;

	return _sconnmanager->FindMostLeisureSlave(id,slave_ip,slave_port);
}
void CConnManager::Execute(Thread::Arg arg)
{
  TcpSocket *new_socket;
  CConnHandler *cconnhandler;
  CConnHandler *new_cconnhandler;
  TcpServerSocket* listening_socket=0;
  tUInt32 counter=0;
  //tSInt64 starttime;
  //tSInt64 endtime;

  //starttime=Timer::GetAbsoluteTime();
  
  LOGINFO("CConnManager starting up...");

  try
  {
	  listening_socket = new TcpServerSocket(_listen_port);
  }
  catch(SocketException &e)
  {
	  LOGINFO("CConnManager::Execute:Failed to instantiate the listening socket,exception:"<<e.Description()<<".");
	  if (listening_socket) 
	  {
		  listening_socket->Disconnect();
		  delete listening_socket;
	  }
	  return;
  }

  /** Start a pool of permanert handlers.*/
  for (tUInt32 i=0;i<_permanent_handler_num;i++)
  {
	  new_cconnhandler = new CConnHandler(this,TRUE,_filelog);
	  new_cconnhandler->Start();
	  _handler_vec.push_back(new_cconnhandler);
  }
  
  while(!_is_shutdown_requested)
   {
	  //For it's time consuming, we do it once every 250 loops.
	  counter++;
	  if (counter==250)
	  {   
          counter=0;
		  CleanShutdownConnections();
	  }
	  
      // Check if there is a new connection request
      // Wait and check within 100 milliseconds
      if (!listening_socket->IsReadyForRead(100))
	  {
	    // No. Then just yield CPU to others
	    // and retry later.
		//LOGDEBUG("CConnManager: not ready for read");
	    Yield();
	    continue;
	  }

      try 
	  {
	    new_socket=listening_socket->Accept();
	  }
      catch(SocketException &e)
	  {
		LOGERROR("CConnManager::Execute:Failed to instanciate socket for new incoming connection:'"<<e.Description()<<"'.");
		continue;
	  }
	  catch (exception &e)
	  {
		LOGERROR("CConnManager::Execute:Failed to instanciate socket for new incoming connection:'"<<e.what()<<"'.");
		continue;
	  }

	  LOGDEBUG("CConnManager::Execute:Received a new connection from '"<<new_socket->PeerHostAddress()<<"'.");
      
	  new_socket->SetKeepAlive();
      
	  cconnhandler = FindMostLeisureHandler();

	  if (!cconnhandler)
	  {
		  new_cconnhandler = new CConnHandler(this,FALSE,_filelog);
		  new_cconnhandler->Start();
		  _handler_vec.push_back(new_cconnhandler);
	  }
	  else if (cconnhandler->CanContainMore())
		  cconnhandler->SaveSocket(new_socket);
	  else
	  {
		  if (_handler_vec.size()<_handler_max)
		  {
			  new_cconnhandler = new CConnHandler(this,FALSE,_filelog);
			  new_cconnhandler->Start();
			  _handler_vec.push_back(new_cconnhandler);

			  new_cconnhandler->SaveSocket(new_socket);
		  }
		  else
			  cconnhandler->SaveSocket(new_socket);	// Ask the handler to forward it to other slave server.
	  }

    } // while(!_is_shutdown_requested)
  
  // Here we received a shutdown signal!

  // Then shuting down all current connections.  
  ShutdownAllConnections();
  
  CleanShutdownConnections();

  //endtime=Timer::GetAbsoluteTime();
  
  //Timer::Shutdown();
  
  LOGINFO("CConnManager shutdowned.");
}

CConnManager::CConnManager(const CConnManager &model)
  : Thread(),
    _listen_port(0),
    _handler_vec(),
    _is_shutdown_requested(FALSE),
    _filelog(NULL)
{
}
void CConnManager::CleanShutdownConnections()
{
  tCConnHandlerIter it;
  CConnHandler* cconnhandler;
  
  it= _handler_vec.begin();

  while(it!=_handler_vec.end())
    {
       if ((*it)->State()!=Thread::FINISHED_STATE)
       {
	      it++;
	      continue;
	   }

       cconnhandler=*it;

       cconnhandler->Join();

       delete cconnhandler;
	   
       it = _handler_vec.erase(it);

    } // while(it!=_handler_vec.end())

}

void CConnManager::ShutdownAllConnections()
{
  tCConnHandlerIter it;
  CConnHandler* cconnhandler;

  for (it=_handler_vec.begin(); it!=_handler_vec.end(); it++)
    {
      cconnhandler=*it;
      
	  cconnhandler->Shutdown();
    }
}
CConnHandler* CConnManager::FindMostLeisureHandler(void) const
{
	tCConnHandlerConstIter it;
	CConnHandler* cconnhandler=NULL;

	if (_handler_vec.empty())
		return NULL;

	it = _handler_vec.begin();

	cconnhandler = (*it++);

	while (it != _handler_vec.end())
	{
		if ((*it)->SocketNum()<cconnhandler->SocketNum())
			cconnhandler = *it;

		it++;
	}

	return cconnhandler;
}