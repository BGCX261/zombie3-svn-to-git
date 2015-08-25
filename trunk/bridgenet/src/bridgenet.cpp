#include "mythread.hxx"
#include "was_api.h"
#include "bridgenet.hxx"

using namespace std;
#if defined(TRACE)
#include "conversion.h"

#if defined(WIN32)
#include <process.h>
#elif defined(linux)
#include <sys/types.h>
#include <unistd.h>
#else
#error "Unsupported platform"
#endif
#endif

#include "myfilelog.hxx"
#include "loghelper.hxx"
#include <assert.h>
#include <string>
#include <iostream>

#include <string.h>	// strlen
#include <malloc.h>	// malloc,memcpy

////////////
BridgeNet::BridgeNet(FileLog *filelog)
  : _incoming_msg_pool(),
    _incoming_msg_size(),
    _incoming_max_size(),
    //_incoming_max_buf_size(),
    _flip_index(0),
    _p(0),
	_first_run(TRUE),
    _filelog(filelog)
{

}


///////////////////////////////////////////////////////////////////////////////
//
////////////
BridgeNet::~BridgeNet()
{
	if (_incoming_msg_pool[0]!=NULL)
	{
		free(_incoming_msg_pool[0]);
	}
	
	if (_incoming_msg_pool[1]!=NULL)
	{
	    free(_incoming_msg_pool[1]);
	}
}

/////////////////////////////////////////////////////////////////////////////////
////
//////////////
//int BridgeNet::Disconnect()
//{
//  if (_remote) 
//	  return _remote->Disconnect();
//  else
//	  return 0;
//}
//tBool BridgeNet::IsConnected(void)
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
//tInt32 BridgeNet::IsReadyForRead() const
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
//const string& BridgeNet::PeerHostAddress(void) const
//{
//	return _remote->PeerHostAddress();
//}
tBool BridgeNet::AnyDataThere(void) const
{
	return (_incoming_msg_size[_flip_index]==0);
}
int BridgeNet::GetMsgLength()
{
  if (!IsHeaderExisted())
    {
      return 0;
    }
  
  return was_get_header_field(_incoming_msg_pool[_flip_index], E_WAS_MSG_HEADER_FIELD_LOB) + HEADERSIZE + TAILSIZE;
}
tBool BridgeNet::IsHeaderExisted()
{
  if (_incoming_msg_size[_flip_index]<HEADERSIZE)
    {
      return FALSE;
    }
  
  //Check and Try to resynchronize it once the header has been received.
  Resynchronize();
  
  return (_incoming_msg_size[_flip_index]>HEADERSIZE);
}
tBool BridgeNet::IsMsgExisted()
{
	int msg_length=GetMsgLength();

	if (!msg_length)
	{
		return FALSE;
	}

	//If the comming msg is too long
	if ((tUInt32)msg_length>_incoming_msg_size[_flip_index]+_incoming_max_size[_flip_index])
	{
		LOGDEBUG("BridgeNet: : #"<<Thread::SelfId()<<":msg length="<<msg_length<<">current buffer length="<<_incoming_msg_size[_flip_index]+_incoming_max_size[_flip_index]<<", So realloc the buffer"<<endl);
		
		_incoming_msg_pool[_flip_index]=(tByte*)realloc(_incoming_msg_pool[_flip_index], msg_length);

		//_incoming_max_buf_size[_flip_index]=msg_length;

		_incoming_max_size[_flip_index]=msg_length-_incoming_msg_size[_flip_index];
		_p=_incoming_msg_pool[_flip_index]+_incoming_msg_size[_flip_index];
	}
	
	//If length of the received is less than the coming msg length
	if (_incoming_msg_size[_flip_index]<(tUInt32)msg_length)
	{
		return FALSE;
	}

	return TRUE;
}
BridgeNet::tBridgeNetStatus BridgeNet::GetAMsgFromBuffer(tBridgeMsg** appmsg)
{
	if (!IsHeaderExisted())
	{
		return BridgeNet::eMsgNotAvailable;
	}

	if (!IsMsgExisted())
	{
		return BridgeNet::eMsgNotAvailable;
	}

	//if already a msg existing in buffer, get the msg.	
	tUInt32 msg_length = GetMsgLength();
	
#if defined(DEBUG)
    LOGDEBUG("BridgeNet: GetAMsgFromBuffer:: #"<<Thread::SelfId()<<":got a msg, msg length="<<msg_length<<" from buffer #"<<_flip_index<<endl);
#endif

	//*appmsg = (tByte*)malloc(msg_length);
	*appmsg = CreateBridgeMsg();

	(*appmsg)->msg = new tByte[msg_length];
	memcpy((*appmsg)->msg, _incoming_msg_pool[_flip_index], msg_length);
	(*appmsg)->lom = msg_length;
	(*appmsg)->mpi = (tUInt32)was_get_header_field((*appmsg)->msg,E_WAS_MSG_HEADER_FIELD_MPI);
	(*appmsg)->tom = (tUInt32)was_get_header_field((*appmsg)->msg,E_WAS_MSG_HEADER_FIELD_TOM);
	(*appmsg)->tid = (tUInt32)was_get_header_field((*appmsg)->msg,E_WAS_MSG_HEADER_FIELD_TID);
	(*appmsg)->rspc= (tUInt32)was_get_header_field((*appmsg)->msg,E_WAS_MSG_HEADER_FIELD_RSPC);

	(*appmsg)->isReceived = TRUE;
	
	//
    //copy the rest to alternative buffer, and flip.
    //
	tUInt32 rest_length = _incoming_msg_size[_flip_index]-msg_length;
    if (rest_length>0)
    {
#if defined(DEBUG)
	  LOGDEBUG("BridgeNet: : #"<<Thread::SelfId()<<":Copy the rest to the other buffer,rest_length="<<rest_length<<endl);
#endif
      //if the length of the rest overflow the other buffer
      if (rest_length > _incoming_max_size[1-_flip_index])
      {
		  LOGDEBUG("BridgeNet::Resynchronize:The rest overflow the other buffer,so reallocate the other."<<endl);
          //_incoming_msg_pool[1-_flip_index] = (tByte*)realloc(_incoming_msg_pool[1-_flip_index],rest_length);
          free(_incoming_msg_pool[1-_flip_index]);
          _incoming_msg_pool[1-_flip_index] = (tByte*)malloc(rest_length);
          _incoming_max_size[1-_flip_index] = rest_length;
	  //_incoming_max_buf_size[1-_flip_index] = rest_length;

      }

      memcpy(_incoming_msg_pool[1-_flip_index],(_incoming_msg_pool[_flip_index]+msg_length),rest_length);
      _incoming_msg_size[1-_flip_index] = rest_length;
      _incoming_max_size[1-_flip_index] = _incoming_max_size[1-_flip_index]-rest_length;
    }
    
	_incoming_max_size[_flip_index] += _incoming_msg_size[_flip_index];
    _incoming_msg_size[_flip_index] = 0;
	  
	_flip_index = 1-_flip_index;

	return BridgeNet::eMsgAvailable;
}

//
///////////////////////////////////////////////////////////////////////////////
//
////////////
void BridgeNet::Clear()
{
  _incoming_max_size[0]+=_incoming_msg_size[0];
  _incoming_msg_size[0]=0;

  _incoming_max_size[1]+=_incoming_msg_size[1];
  _incoming_msg_size[1]=0;

  _flip_index=0;
  _p=(tByte*)_incoming_msg_pool[0];
}

void BridgeNet::Resynchronize()
{
    tUInt32 recvedlength=_incoming_msg_size[_flip_index];
	
	E_WAS_MSG_VALIDITY res=was_is_msg_valid(_incoming_msg_pool[_flip_index], &recvedlength);
	
    if (res!=E_WAS_MSG_INVALID_TOO_SHORT &&
			res!=E_WAS_MSG_VALID)
    {
        tUInt32 msgoffset=0;
        tUInt32 msglength=0;
        tUInt32 bufferlen=(_incoming_msg_size[_flip_index]+_incoming_max_size[_flip_index]);

		LOGWARNING("BridgeNet::Resynchronize:The received info is of illegal header, resynchronize it!");

        if (E_WAS_MSG_NOT_SYNC!=was_resync(_incoming_msg_pool[_flip_index],_incoming_msg_size[_flip_index],&msgoffset, &msglength))
        {
            ///
            //At least, "SOBM" was found, So abandon the info before "SOBM"
            ///

            //Move the valid info to the beginning of the current buffer
            //tUInt32 leftinfosize=_incoming_msg_size[_flip_index]-msgoffset;
            memmove(_incoming_msg_pool[_flip_index],
                    (_incoming_msg_pool[_flip_index]+msgoffset),(_incoming_msg_size[_flip_index]-msgoffset));

            _incoming_msg_size[_flip_index]-=msgoffset;
            _incoming_max_size[_flip_index]+=msgoffset;

            _p-=msgoffset;

			LOGDEBUG("BridgeNet::Resynchronize:Successfully resynchronized!");
        }
        else
        {
            //if can not find 'SOBM', abondon all the info except the last 4 bytes in the current buffer
            memmove(_incoming_msg_pool[_flip_index],
                    (_incoming_msg_pool[_flip_index]+_incoming_msg_size[_flip_index]-4),4);

            _incoming_msg_size[_flip_index]=4;
            _incoming_max_size[_flip_index]=bufferlen-4;

            _p=(tByte*)(_incoming_msg_pool[_flip_index]+4);

			LOGWARNING("BridgeNet::Resynchronize:Resynchronization failed! so nly the last 4 bytes kept!");
        }
    }
}