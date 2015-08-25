/****************************************************************************************
* For receiving and sending message														*
* Copyright (c) 2009-2010																*
* Author	:	dongwen																	*
* Date		:	20100401																*
*****************************************************************************************/

#ifndef _UDP_BRIDGENET_HXX_
#define _UDP_BRIDGENET_HXX_


#include "mydll.h"
#include "mytypes.h"
#include "bridgenet.hxx"

class UdpSocket;

class EXPORT_BRIDGENET UdpBridgeNet	:	public BridgeNet
{
public:
	//
  UdpBridgeNet(FileLog *filelog=0);

  virtual ~UdpBridgeNet();

  virtual tBool SendBridgeMsg(tBridgeMsg* appmsg);
  
  //Every call get one msg and meanwhile make sure the content of next msg not loss.
  //If more than one messages are sent by one sending, IsReadyForRead() should not be called
  //prior to this method for the rest messages other than the first are maybe already stored
  //in the buffer, just call ReceiveMsg() to get them from buffer.
  virtual tBridgeNetStatus ReceiveMsg(tBridgeMsg** appmsg);
  
  //int Disconnect(void);
  //tBool IsConnected(void);

  virtual tInt32 IsReadyForWrite() const;
  //tInt32 IsReadyForRead() const;
  //const string& PeerHostAddress(void) const;

  void SetUdpSocket(UdpSocket* remote);

  UdpSocket* GetUdpSocket(void);

private:
  //
  // Socket from which we will receive msg and to which we will send messages.
  UdpSocket *_remote;

};

#endif // _UDP_BRIDGENET_HXX_
