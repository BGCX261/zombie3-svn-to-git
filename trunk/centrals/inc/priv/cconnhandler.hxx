#ifndef __CHATTER_CONN_HANDLER__
#define __CHATTER_CONN_HANDLER__

#include <map>
#include "mytypes.h"
#include "priv/connhandler.hxx"

class CConnManager;
class TcpSocket;
class CacherDB;

class CConnHandler	:	public ConnHandler
{
public:
	//
	//////////////////////////////////TYPEDEFS////////////////////////////////////////////////////
	typedef map<tUInt32,TcpSocket*>			tIdSocketMap;
	typedef tIdSocketMap::iterator			tIdSocketIter;
	typedef tIdSocketMap::const_iterator	tIdSocketConstIter;
	typedef pair<tUInt32,TcpSocket*>		tIdSocketPair;

	typedef map<tUInt32,TcpBridgeNet*>		tIdBridgeNetMap;
	typedef tIdBridgeNetMap::iterator		tIdBridgeNetIter;
	typedef tIdBridgeNetMap::const_iterator	tIdBridgeNetConstIter;
	typedef pair<tUInt32,TcpBridgeNet*>		tIdBridgeNetPair;
	//////////////////////////////////////////////////////////////////////////////////////////////

	/**
	 *\brief	Constructor
	 * @param remote_host	:	the address to connect to
	 * @param port			:	the remote listening tcp port
	 */
	CConnHandler(CConnManager* manager,tBool permanent,FileLog* filelog);

	/**
	 * \brief	Destructor
	 */
	virtual ~CConnHandler();

	tBool	CanContainMore(void) const;
	tUInt32	SocketNum(void) const;
	void	SaveSocket(TcpSocket* socket);

	tBool	ShallShutdown(void) const;

protected:
	//
	virtual tBool RecvBridgeMsg(void);
	virtual tBool SendBridgeMsg(void);
	virtual tBool ProcessIMsg(void);
	virtual tBool DoOTask(void);

private:
	//
	tUInt32 GetASocketId(void);
	TcpBridgeNet* GetMyOwnBridgeNet(tUInt32 id) const;
	void SaveMyOwnBridgeNet(tUInt32 id,TcpBridgeNet* bridgenet);
	void RemoveMyOwnBridgeNet(tUInt32 id);

	void ProcessLoginREQMsg(tBridgeMsg* imsg);
	void ProcessInviteVoiceREQMsg(tBridgeMsg* imsg);
	void ProcessInviteVoiceCNFMsg(tBridgeMsg* imsg);

	void RegiesterLoginMember(tUInt32 id,string name,string public_ip,tUInt32 public_tcp_port,string internal_ip,tUInt32 internal_tcp_port);

	TcpSocket* FindSocket(tUInt32 id) const;
	
	tUInt32 _socket_id_counter;	//For creating a unique Id within a handler.
	tIdSocketMap _id_socket_map;	//Storing connection for users.
	tIdSocketMap _logined_id_socket_map;
	tIdBridgeNetMap _id_bridgenet_map;	//Connecting a bridgenet to a connection to a specific user.

	CConnManager* _manager;
	CacherDB*	_cacher;
	tBool	_permanent;
};
#endif //__CHATTER_CONN_HANDLER__