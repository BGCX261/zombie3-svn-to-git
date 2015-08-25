#ifndef __CTHANDLER_HXX__
#define __CTHANDLER_HXX__

#include <map>
#include "mytypes.h"
#include "priv/connhandler.hxx"

class SecUdpNeter;
class CacherDB;

class CTHandler	:	public ConnHandler
{
public:
	//
	//////////////////////////////////TYPEDEFS////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////

	/**
	 *\brief	Constructor
	 * @param remote_host	:	the address to connect to
	 * @param port			:	the remote listening tcp port
	 */
	CTHandler(tUInt32 listen_port,FileLog* filelog);

	/**
	 * \brief	Destructor
	 */
	virtual ~CTHandler();

	tBool	CanContainMore(void) const;

protected:
	//
	virtual tBool RecvBridgeMsg(void);
	virtual tBool SendBridgeMsg(void);
	virtual void PreRunning();
	virtual void AfterRunning();
	virtual tBool ProcessIMsg(void);
	virtual tBool DoOTask(void);

private:
	//
	void ProcessLoginReq(tBridgeMsg* imsg);
	void ProcessLogoffReq(tBridgeMsg* imsg);
	void ProcessRegisterUdpEndpoints(tBridgeMsg* imsg);
	void ProcessChatReq(tBridgeMsg* imsg);
	void ProcessChatCnf(tBridgeMsg* imsg);
	void ForwardMsg(tBridgeMsg* imsg);
	
	void RegiesterLoginMember(tUInt32 id,string name,string public_ip,tUInt32 public_tcp_port,string internal_ip,tUInt32 internal_tcp_port);

	SecUdpNeter*_secudpneter;
	CacherDB*	_cacher;
	tUInt32		_listen_port;
};
#endif //__CTHANDLER_HXX__