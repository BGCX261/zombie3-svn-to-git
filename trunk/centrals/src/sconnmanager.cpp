#include "mythread.hxx"
#include "mytimer.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "socketexception.hxx"
#include "tcpserversocket.hxx"
#include "priv/sconnhandler.hxx"
#include "priv/sconnmanager.hxx"

SConnManager::SConnManager(tUInt32 sport,FileLog *filelog)
  : Thread(),
    _id_sconn_map(),
	_handler_vec(),
    _sport(sport),
    _is_shutdown_requested(FALSE),
    _filelog(filelog)
{
}

SConnManager::~SConnManager()
{
}
void SConnManager::Shutdown()
{
  _is_shutdown_requested=TRUE;
}
tBool SConnManager::FindMostLeisureSlave(tUInt32& id,string& slave_host,tUInt32& slave_port) const
{
	SConnHandler* sconnhandler;
	tIdSConnMapConstIter it;

	if (_id_sconn_map.empty())
		return FALSE;

	it = _id_sconn_map.begin();
	sconnhandler = (it++)->second;

	while (it != _id_sconn_map.end())
	{
		if (!it->second->GetSlaveHost().empty() && it->second->GetSlavePort()!=0 && it->second->GetSlaveLoad()<sconnhandler->GetSlaveLoad())
			sconnhandler = it->second;

		it++;
	}

	id = sconnhandler->GetSlaveId();
	slave_host = sconnhandler->GetSlaveHost();
	slave_port = sconnhandler->GetSlavePort();

	return (!slave_host.empty() && slave_port!=0 && id!=0);
}
void SConnManager::Execute(Thread::Arg arg)
{
  TcpSocket* new_socket;
  SConnHandler* new_handler;
  TcpServerSocket* listening_socket=0;
  tUInt32 counter=0;
  //tSInt64 starttime;
  //tSInt64 endtime;

  //starttime=Timer::GetAbsoluteTime();
  
  LOGINFO("SConnManager starting up...");

  try
  {
	  listening_socket = new TcpServerSocket(_sport);
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
		//LOGDEBUG("SConnManager: not ready for read");
	    Yield();
	    continue;
	  }

      try 
	  {
	    new_socket=listening_socket->Accept();
	  }
      catch(SocketException &e)
	  {
		LOGERROR("SConnManager::Execute:Failed to instanciate socket for new incoming connection:'"<<e.Description()<<"'.");
		continue;
	  }
	  catch (exception &e)
	  {
		LOGERROR("SConnManager::Execute:Failed to instanciate socket for new incoming connection:'"<<e.what()<<"'.");
		continue;
	  }

	  LOGDEBUG("SConnManager::Execute:Received a new connection from '"<<new_socket->PeerHostAddress()<<"'.");
      
	  new_socket->SetKeepAlive();
      
	  new_handler=new SConnHandler(this,new_socket,_filelog);

	  new_handler->Start();
	  
	  _handler_vec.push_back(new_handler);

    } // while(!_is_shutdown_requested)
  
  // Here we received a shutdown signal!

  // Then shuting down all current connections.  
  ShutdownAllConnections();
  
  CleanShutdownConnections();

  //endtime=Timer::GetAbsoluteTime();
  
  //Timer::Shutdown();
  
  LOGINFO("SConnManager shutdowned.");
}

SConnManager::SConnManager(const SConnManager &model)
  : Thread(),
    _sport(0),
    _handler_vec(),
    _is_shutdown_requested(FALSE),
    _filelog(NULL)
{
}
void SConnManager::CleanShutdownConnections()
{
  tSConnHandlerIter it;
  SConnHandler* handler;
  
  it= _handler_vec.begin();

  while(it!=_handler_vec.end())
    {
       if ((*it)->State()!=Thread::FINISHED_STATE)
       {
	      it++;
	      continue;
	   }

       handler=*it;

       it = _handler_vec.erase(it);

       handler->Join();

       delete handler;

    } // while(it!=_handler_vec.end())
}

void SConnManager::ShutdownAllConnections()
{
  tSConnHandlerIter it;
  SConnHandler* handler;

  for (it=_handler_vec.begin(); it!=_handler_vec.end(); it++)
    {
      handler=*it;
      
	  handler->Shutdown();
    }
}
