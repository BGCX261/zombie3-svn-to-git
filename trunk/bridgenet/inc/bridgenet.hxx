/****************************************************************************************
* For receiving and sending message														*
* Copyright (c) 2009-2010																*
* Author	:	dongwen																	*
* Date		:	20100401																*
*****************************************************************************************/

#ifndef _BRIDGENET_HXX_
#define _BRIDGENET_HXX_

//#include "mydll.h"
#include "was_api.h"
#include "mytypes.h"
#include "bridgemessage.hxx"

#ifdef WIN32

#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"mythread")
#pragma comment(lib,"myfile")
#pragma comment(lib,"mynet")
#pragma comment(lib,"myudpnet")
#pragma comment(lib,"message")

#else
#define EXPORT_BRIDGENET 
#endif

#define INIMSGBUFSIZE	1800

class FileLog;

class EXPORT_BRIDGENET BridgeNet
{
public:

  typedef enum
    {
      eUnknown=0,
      eServiceKnown,
      eMsgAvailable,
      eMsgNotAvailable,
	  eNotConnected,
      eEndOfConnectionReceived,
      eNetworkErrorReceived,
      eCorruptedMessage
    } tBridgeNetStatus;

  BridgeNet(FileLog *filelog=0);

  virtual ~BridgeNet();

  virtual tBool SendBridgeMsg(tBridgeMsg* appmsg) = 0;
  
  //Every call get one msg and meanwhile make sure the content of next msg not loss.
  //If more than one messages are sent by one sending, IsReadyForRead() should not be called
  //prior to this method for the rest messages other than the first are maybe already stored
  //in the buffer, just call ReceiveMsg() to get them from buffer.
  virtual tBridgeNetStatus ReceiveMsg(tBridgeMsg** appmsg)=0;
  
  //int Disconnect(void);
  //tBool IsConnected(void);

  virtual tInt32 IsReadyForWrite() const = 0;
  //tInt32 IsReadyForRead() const;
  //const string& PeerHostAddress(void) const;

  virtual tBool AnyDataThere(void) const;

protected:
  //
  tInt32 GetMsgLength();
   
  tBool IsHeaderExisted();
  
  //1.Ckeck if a complete msg already exists in buffer
  //2.If length of the coming msg exceeds buffer size, adjust buffer size.
  tBool IsMsgExisted();

  //Get a msg from buffer and flip buffer  
  tBridgeNetStatus GetAMsgFromBuffer(tBridgeMsg** msg);

  //Clear current buffers.
  void Clear();

  void Resynchronize();

  //tBool ValidateHeader(tByte* appmsg);
  //tBool ValidateMessage(tByte* appmsg);
  
  
  // Buffer for incoming messages.
  // We can only get one msg at a time 
  // with eventually (the start of) the next
  // msg. Since we process the incoming
  // msg in the same thread upon reception, 
  // this should be enough.
  tByte* _incoming_msg_pool[2]; 
  
  // Size of incoming messages.
  tUInt32 _incoming_msg_size[2]; 
  
  // Maximum allowed bytes that can still be 
  // appended to a given msg.
  tUInt32 _incoming_max_size[2]; 

  // Maximum allowed bytes that current buffer can contain.
  //tUInt32 _incoming_max_buf_size[2]; 

  // Flip_Index will alternatively take the values 1, 0, 
  // 1, 0, 1, ... so that we will be able to flip the 
  // following buffers: incoming_msg_pool, 
  // incoming_msg_size and incoming_max_size.
  tUInt32 _flip_index;

  // Pointer that will dynamically points on the memory location
  // of a msg to write the receive data.
  tByte* _p;
  
  tBool _first_run;
	
  FileLog *_filelog;
};

#endif // _BRIDGENET_HXX_
