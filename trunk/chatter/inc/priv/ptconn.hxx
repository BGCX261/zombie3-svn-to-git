/********************************************************************************************
 * Connection to Peer Member																*
 ********************************************************************************************/

#ifndef __PTCONN_HXX__
#define __PTCONN_HXX__

#include "mydll.h"
#include "mytypes.h"
#include "priv/tconn.hxx"

class OTConnManager;

class PTConn	:	public TConn
{
public:
	
	/**
	 *\brief	:	Constructor
	 * @param manager	:
	 * @param remote_public_host	:	the public ip to connect to
	 * @param remote_public_port	:	the public listening tcp port
	 * @param remote_internal_host	:	For hole punching,we need public and internal endpoints both.
	 * @param remote_internal_port	:
	 */
	PTConn(OTConnManager* manager,string remote_public_host,tUInt32 remote_public_port,
			string remote_internal_host,tUInt32 remote_internal_port,FileLog* filelog = NULL);

	/**
	 * @brief	:	Destructor
	 */
	virtual ~PTConn();

	tUInt32	Transfer(string file_name_and_path);

	/**
	 * building a bridge net to peer, by hole punching.
	 */
	TcpBridgeNet*	PHoleBuildBridgeNet(void);
	
protected:
	//
	//virtual void Execute(Thread::Arg arg);
	//virtual tBool HandleIncomming(void);
	//virtual tBool HandleOutgoing(void);
	virtual tBool ProcessIMsg(void);

	/**
	 * Check if any out going task to do, and do it
	 */
	virtual tBool DoOTask(void);

	void ProcessLoginCNFMsg(tBridgeMsg* imsg);
	void ProcessJoinVoiceChatCNFMsg(tBridgeMsg* imsg);

private:
	//
	OTConnManager* _manager;

	string	_remote_internal_host;
	tUInt32 _remote_internal_port;
};
#endif //__PTCONN_HXX__