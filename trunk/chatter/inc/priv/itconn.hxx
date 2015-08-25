#ifndef __CONN_HANDLER_HXX__
#define __CONN_HANDLER_HXX__
#include "mythread.hxx"
#include "safeque.hxx"
#include "bridgenet.hxx"
#include "priv/tconn.hxx"
#include "taskdefs.h"
#include "priv/groupmgr.hxx"
#include "priv/itconnmanager.hxx"

class TcpSocket;

class ITConn : public TConn
{
public:
  ////////////////////////////////////////TYPEDEFS//////////////////////////////
  typedef SafeQue<tBridgeMsg*> tBridgeMsgQue;

  // \brief constructor
  ITConn(ITConnManager* connmanager,TcpSocket* remote,FileLog *filelog);

  //\brief descructor
  virtual ~ITConn();

  /**
   * @brief	: reply the request from peer,normally triggered by user's action with decision.
   * @param tid		: the id of the request task
   * @param desicion	: my decision to join or not
   */
  void ReplyInvitePeerRequest(tUInt32 tid,tBool decision = TRUE);

protected:
	//
	virtual tBool ProcessIMsg(void);
	virtual tBool DoOTask(void);  
private: 
  //
  ITConn(const ITConn &model);

  /**
   * @bried	: Confirm the login request with its own public ip and port.For Central Server
   * @param	: the request message
   */
  void ProcessIMsgLogin(tBridgeMsg* imsg);

  /**
   * @bried	: Tell the user the ip and port it requested.For Central Server
   * @param	: the request message
   */
  void ProcessIMsgMemInfoReq(tBridgeMsg* imsg);

  /**
   * @brief	: register the request at the status list
   * @param	: the request message
   */
  void ProcessIMsgInvitePeer(tBridgeMsg* imsg);

  /**
   * @brief	: normally don't register AddMember request, but reply to it with positive answer
   * @param	imsg	: the request message
   */
  void ProcessIMsgAddMember(tBridgeMsg* imsg);

  /**
   * @brief	: register the request at the status list
   * @param	: the request message
   */
  void ProcessIMsgTransferReqest(tBridgeMsg* imsg);

  /**
   * @brief	: save the received data into local file and update the task status
   * @param imsg	: the received sending data message
   I*/
  void ProcessIMsgTransferData(tBridgeMsg* imsg);

  void			RegisterTransferingFile(tTransFile* tfile);
  tTransFile*	GetTransferingFile(tUInt32 tid);

  ITConnManager* _manager;

  tTransFile*	_tfile_list;
};

#endif //__CONN_HANDLER_HXX__
