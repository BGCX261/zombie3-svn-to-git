#include "mythread.hxx"
#include "mytimer.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "priv/itconn.hxx"
#include "priv/itconnmanager.hxx"

ITConnManager::ITConnManager(tUInt32 listen_port,FileLog *filelog)
  : Thread(),
    _connection_vec(),
    _listen_port(listen_port),
    _is_shutdown_requested(FALSE),
    _filelog(filelog)
{
}
ITConnManager::~ITConnManager()
{
}

void ITConnManager::Shutdown()
{
  _is_shutdown_requested=true;
}
void ITConnManager::Execute(Thread::Arg arg)
{
  TcpServerSocket* listening_socket=NULL;
  TcpSocket *new_socket;
  ITConn *new_www_conn;
  tUInt32 counter=0;
  //tSInt64 starttime;
  //tSInt64 endtime;

  //starttime=Timer::GetAbsoluteTime();
  
  LOGINFO("ITConnManager starting up...");

  try
  {
      listening_socket = new TcpServerSocket(_listen_port);
  }
  catch(SocketException &e)
  {
	  LOGERROR("ITConnManager::Init:Exception happened while instantiating listening socket:"<<e.Description());
	  Shutdown();
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
	    Yield();
	    continue;
	  }

      try 
	  {
	    new_socket=listening_socket->Accept();
	  }
      catch(SocketException &e)
	  {
		LOGERROR("ITConnManager::Execute:Failed to instanciate socket for new incoming connection:'"<<e.Description()<<"'.");
		continue;
	  }
	  catch (exception &e)
	  {
		LOGERROR("ITConnManager::Execute:Failed to instanciate socket for new incoming connection:'"<<e.what()<<"'.");
		continue;
	  }

	  LOGDEBUG("ITConnManager::Execute:Received a new connection from '"<<new_socket->PeerHostAddress()<<"::"<<new_socket->PeerHostPort()<<"'.");
      
	  new_socket->SetKeepAlive();
      
	  new_www_conn=new ITConn(this,new_socket,_filelog);

	  // Starting thread to handle newly received connection.
	  new_www_conn->Start();
	  
	  _connection_vec.push_back(new_www_conn);

    } // while(!_is_shutdown_requested)
  
  // Here we received a shutdown signal!

  if (listening_socket)
  {
	  listening_socket->Disconnect();
	  delete listening_socket;
	  listening_socket = NULL;
  }

  // Then shuting down all current connections.  
  ShutdownAllConnections();
  
  CleanShutdownConnections();

  //endtime=Timer::GetAbsoluteTime();
  
  //Timer::Shutdown();
  
  LOGINFO("ITConnManager shutdowned.");
}

ITConnManager::ITConnManager(const ITConnManager &model)
  : Thread(),
    _listen_port(0),
    _connection_vec(),
    _is_shutdown_requested(FALSE),
    _filelog(NULL)
{
}
void ITConnManager::CleanShutdownConnections()
{
  tITConnVec::iterator it;
  TConn* itconn;
  
  it= _connection_vec.begin();

  while(it!=_connection_vec.end())
    {
       if ((*it)->State()!=Thread::FINISHED_STATE)
       {
	      it++;
	      continue;
	   }

       itconn=*it;

       it = _connection_vec.erase(it);

       itconn->Join();

       delete itconn;

    } // while(it!=_connection_vec.end())

}

void ITConnManager::ShutdownAllConnections()
{
  tITConnVec::iterator it;
  TConn* itconn;

  for (it=_connection_vec.begin(); it!=_connection_vec.end(); it++)
    {
      itconn=*it;
      
	  itconn->Shutdown();
    }
}
