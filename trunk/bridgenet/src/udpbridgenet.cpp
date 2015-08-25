#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "mythread.hxx"
#include "udpsocket.hxx"
#include "udpbridgenet.hxx"
#include "bridgenet.hxx"

////////////
UdpBridgeNet::UdpBridgeNet(FileLog *filelog)
  : BridgeNet(filelog)
{
}

///////////////////////////////////////////////////////////////////////////////
//
////////////
UdpBridgeNet::~UdpBridgeNet()
{
}

/////////////////////////////////////////////////////////////////////////////////
////
//////////////
//int UdpBridgeNet::Disconnect()
//{
//  if (_remote) 
//	  return _remote->Disconnect();
//  else
//	  return 0;
//}
//tBool UdpBridgeNet::IsConnected(void)
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
tInt32 UdpBridgeNet::IsReadyForWrite() const
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
//tInt32 UdpBridgeNet::IsReadyForRead() const
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
//const string& UdpBridgeNet::PeerHostAddress(void) const
//{
//	return _remote->PeerHostAddress();
//}
void UdpBridgeNet::SetUdpSocket(UdpSocket* remote)
{
	/* Since connection changed, previously received data is valueless, so abandon it.*/
	Clear();

    _remote=remote;
}
UdpSocket* UdpBridgeNet::GetUdpSocket(void)
{
	return _remote;
}
//
///////////////////////////////////////////////////////////////////////////////
//
////////////
tBool UdpBridgeNet::SendBridgeMsg(tBridgeMsg* appmsg)
{
  int res;

  // Before sending, make sure the socket will accept some data for sending.
  if (!_remote || !_remote->IsReadyForWrite())
    {
      LOGWARNING("UdpBridgeNet::SendBridgeMsg: #"<<Thread::SelfId()<<": Sending attempted while socket not ready for write.");
      return FALSE;
    }
  
  res=_remote->SendTo(appmsg->msg, appmsg->lom,appmsg->rhost,appmsg->rport);
  
  if (res<0) 
  {
#if defined(DEBUG)
      LOGWARNING("UdpBridgeNet::SendBridgeMsg: #"<<Thread::SelfId()<<": Sending attempt failed from TCP/IP socket.");
#else
	  LOGWARNING("UdpBridgeNet::SendBridgeMsg:Sending attempt failed from TCP/IP socket.");
#endif
  }
  else
  {
	  //Check if have sent all need to send
	  if (res != appmsg->lom)
	  {
#if defined(DEBUG)
      LOGWARNING("UdpBridgeNet::SendBridgeMsg: #"<<Thread::SelfId()<<": Not sent all the msg.");
#else
		  LOGWARNING("UdpBridgeNet::SendBridgeMsg:Not sent all the msg.");
#endif
	      res = -1;
	  }
	  else
	  {
#if defined(DEBUG)
      //LOGDEBUG("UdpBridgeNet::SendBridgeMsg:Sending successfully done.");
#else
		  //LOGDEBUG("UdpBridgeNet::SendBridgeMsg:Sending successfully done.");
#endif
	  }
  }

  return res>0;
}


UdpBridgeNet::tBridgeNetStatus UdpBridgeNet::ReceiveMsg(tBridgeMsg** appmsg)
{
  string rhost;
  tInt32 rport;

  int currently_read_byte; // Actual bytes number read from socket stream.

  if (!_remote)
	  return eNotConnected;

  if (_first_run)
  {
      LOGDEBUG("UdpBridgeNet::ReceiveMsg:First run initialization.");
      
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
      //LOGDEBUG("UdpBridgeNet::ReceiveMsg: #"<<Thread::SelfId()<<" "<<_incoming_msg_size[_flip_index]<<" bytes already in buffer #"<<_flip_index<<endl);

      //Trying to get msg from buffer.
      if (UdpBridgeNet::eMsgAvailable==GetAMsgFromBuffer(appmsg))
        {
			// think about it later.
			//(*appmsg)->rhost = rhost;
			//(*appmsg)->rport = rport;
		  
			return UdpBridgeNet::eMsgAvailable;
		}
      
      _p=(tByte *) _incoming_msg_pool[_flip_index] + _incoming_msg_size[_flip_index];
      
    }//if (_first_run)

  while (_remote->IsReadyForRead())
  {

    // Blocking read occurs here.
    currently_read_byte=_remote->RecvFrom(_p,(int)_incoming_max_size[_flip_index],rhost,rport);

    if (currently_read_byte==0)
    {
	    //LOGWARNING("Thread #"<<Thread::SelfId()<<": End of connection received by peer. unexpectedly shutdown.");
      
        return UdpBridgeNet::eEndOfConnectionReceived;
    }
    
    if (currently_read_byte<0)
    {
	    LOGWARNING("UdpBridgeNet::ReceiveMsg:Unexpectedly connection terminated.");
	    
        return UdpBridgeNet::eNetworkErrorReceived;
    }
  
    // Here we successfully read some data on the socket. Storing them ...
    _incoming_msg_size[_flip_index]+=currently_read_byte;
    _incoming_max_size[_flip_index]-=currently_read_byte;
    _p += currently_read_byte;

    LOGDEBUG("UdpBridgeNet::ReceiveMsg:This loop Received "<<currently_read_byte<<" bytes.");
 
    if (UdpBridgeNet::eMsgAvailable==GetAMsgFromBuffer(appmsg))
    {
		//  if (IsShutdownInd(*appmsg))
		//{
		//	  LOGDEBUG("UdpBridgeNet::ReceiveMsg: #"<<Thread::SelfId()<<": Received a shutdown indication,connection shutdown normally"<<endl);
		//  
		//  return UdpBridgeNet::eEndOfConnectionReceived;
		//}

		(*appmsg)->rhost = rhost;
		(*appmsg)->rport = rport;

      return UdpBridgeNet::eMsgAvailable;
    }
  }//while(_remote->IsReadyForRead())

  return UdpBridgeNet::eMsgNotAvailable;
}

