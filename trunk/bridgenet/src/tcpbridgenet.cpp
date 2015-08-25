#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "mythread.hxx"
#include "tcpsocket.hxx"
#include "tcpbridgenet.hxx"
#include "bridgenet.hxx"

////////////
TcpBridgeNet::TcpBridgeNet(FileLog *filelog)
  : BridgeNet(filelog)
{
}


///////////////////////////////////////////////////////////////////////////////
//
////////////
TcpBridgeNet::~TcpBridgeNet()
{
}

/////////////////////////////////////////////////////////////////////////////////
////
//////////////
//int TcpBridgeNet::Disconnect()
//{
//  if (_remote) 
//	  return _remote->Disconnect();
//  else
//	  return 0;
//}
//tBool TcpBridgeNet::IsConnected(void)
//{
//	if (!_remote)
//		return FALSE;
//
//	return _remote->IsConnected();
//}
//
/////////////////////////////////////////////////////////////////////////////////
////
//////////////
tInt32 TcpBridgeNet::IsReadyForWrite() const
{
  if (_remote)
    {
      return _remote->IsReadyForWrite();
    }
  else
    {
      return 0;
    }
}
//tInt32 TcpBridgeNet::IsReadyForRead() const
//{
//  if (_remote)
//    {
//      return _remote->IsReadyForRead();
//    }
//  else
//    {
//      return 0;
//    }
//}
//const string& TcpBridgeNet::PeerHostAddress(void) const
//{
//	return _remote->PeerHostAddress();
//}
void TcpBridgeNet::SetTcpSocket(TcpSocket* remote)
{
	//Since connection changed, previously received
	//data is valueless, so abandon it.
	Clear();

    _remote=remote;
}
TcpSocket* TcpBridgeNet::GetTcpSocket(void)
{
	return _remote;
}
//
///////////////////////////////////////////////////////////////////////////////
//
////////////
tBool TcpBridgeNet::SendBridgeMsg(tBridgeMsg* appmsg)
{
  int res;

  // Before sending, make sure the socket will accept some data for sending.
  if (!_remote || !_remote->IsReadyForWrite())
    {
      LOGWARNING("TcpBridgeNet::SendBridgeMsg: #"<<Thread::SelfId()<<": Sending attempted while socket not ready for write.");
      return FALSE;
    }
  
  res=_remote->Send((char*)appmsg->msg, appmsg->lom);
  
  if (res<0) 
  {
#if defined(DEBUG)
      LOGWARNING("TcpBridgeNet::SendBridgeMsg: #"<<Thread::SelfId()<<": Sending attempt failed from TCP/IP socket.");
#else
	  LOGWARNING("TcpBridgeNet::Thread #"<<Thread::SelfId()<<": Sending attempt failed from TCP/IP socket.");
#endif
  }
  else
  {
	  //Check if have sent all need to send
	  if (res != appmsg->lom)
	  {
#if defined(DEBUG)
      LOGWARNING("TcpBridgeNet::SendBridgeMsg: #"<<Thread::SelfId()<<": Not sent all the msg.");
#else
		  LOGWARNING("TcpBridgeNet::Thread #"<<Thread::SelfId()<<": Not sent all the msg.");
#endif
	      res = -1;
	  }
	  else
	  {
#if defined(DEBUG)
      LOGDEBUG("TcpBridgeNet::SendBridgeMsg: #"<<Thread::SelfId()<<": Sending successfully done.");
#else
		  LOGDEBUG("TcpBridgeNet::Thread #"<<Thread::SelfId()<<": Sending successfully done.");
#endif
	  }
  }

  return res>0;
}

BridgeNet::tBridgeNetStatus TcpBridgeNet::ReceiveMsg(tBridgeMsg** appmsg)
{
  int currently_read_byte; // Actual bytes number read from socket stream.

  if (!_remote)
	  return eNotConnected;

  if (_first_run)
  {
      LOGDEBUG("TcpBridgeNet::ReceiveMsg:First run initialization.");
      
      _first_run=FALSE;
	  
      //Allocate for incoming msg buffer
      _incoming_msg_pool[_flip_index] = (tByte*)malloc(INIMSGBUFSIZE);
      _incoming_msg_pool[1-_flip_index] = (tByte*)malloc(INIMSGBUFSIZE);

      //_incoming_max_buf_size[_flip_index]=INIMSGBUFSIZE;
      //_incoming_max_buf_size[1-_flip_index]=INIMSGBUFSIZE;

      _incoming_msg_size[_flip_index]=0;
      _incoming_max_size[_flip_index]=INIMSGBUFSIZE;
	  
      _incoming_msg_size[1-_flip_index]=0;
      _incoming_max_size[1-_flip_index]=INIMSGBUFSIZE;
      
      _p=(tByte*)_incoming_msg_pool[_flip_index];
    }
  else
    {
      //LOGDEBUG("TcpBridgeNet::ReceiveMsg: #"<<Thread::SelfId()<<" "<<_incoming_msg_size[_flip_index]<<" bytes already in buffer #"<<_flip_index<<endl);

      //Trying to get msg from buffer.
      if (TcpBridgeNet::eMsgAvailable==GetAMsgFromBuffer(appmsg))
        {
		  //if (IsShutdownInd(*appmsg))
				//{
			 // LOGDEBUG("TcpBridgeNet::ReceiveMsg: #"<<Thread::SelfId()<<": Received a shutdown indication,connection shutdown normally"<<endl);
		  //    
			 // return TcpBridgeNet::eEndOfConnectionReceived;
				//}
			(*appmsg)->rhost = _remote->PeerHostAddress();
			(*appmsg)->rport = _remote->PeerHostPort();
		  
			return TcpBridgeNet::eMsgAvailable;
		}
      
      _p=(tByte *) _incoming_msg_pool[_flip_index] + _incoming_msg_size[_flip_index];
      
    }//if (_first_run)

  while (_remote->IsReadyForRead())
  {

    // Blocking read occurs here.
    currently_read_byte=_remote->Recv((char*)_p,(int)_incoming_max_size[_flip_index],&currently_read_byte);

    if (currently_read_byte==0)
    {
	    //LOGWARNING("Thread #"<<Thread::SelfId()<<": End of connection received by peer. unexpectedly shutdown.");
      
        return TcpBridgeNet::eEndOfConnectionReceived;
    }
    
    if (currently_read_byte<0)
    {
	    LOGWARNING("TcpBridgeNet::ReceiveMsg:Unexpectedly connection terminated.");
	    
        return TcpBridgeNet::eNetworkErrorReceived;
    }
  
    // Here we successfully read some data on the socket. Storing them ...
    _incoming_msg_size[_flip_index]+=currently_read_byte;
    _incoming_max_size[_flip_index]-=currently_read_byte;
    _p += currently_read_byte;

    LOGDEBUG("TcpBridgeNet::ReceiveMsg:This loop Received "<<currently_read_byte<<" bytes.");
 
    if (TcpBridgeNet::eMsgAvailable==GetAMsgFromBuffer(appmsg))
    {
		//  if (IsShutdownInd(*appmsg))
		//{
		//	  LOGDEBUG("TcpBridgeNet::ReceiveMsg: #"<<Thread::SelfId()<<": Received a shutdown indication,connection shutdown normally"<<endl);
		//  
		//  return TcpBridgeNet::eEndOfConnectionReceived;
		//}
      (*appmsg)->rhost = _remote->PeerHostAddress();
	  (*appmsg)->rport = _remote->PeerHostPort();

      return TcpBridgeNet::eMsgAvailable;
    }
  }//while(_remote->IsReadyForRead())

  return TcpBridgeNet::eMsgNotAvailable;
}

