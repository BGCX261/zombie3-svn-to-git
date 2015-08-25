#ifndef __NOTIFIER_HXX__
#define __NOTIFIER_HXX__

#include "mytypes.h"
#include "otconn.hxx"
#include "bridgemessage.h"

class Notifier	:	public OTConn
{
public:
	/**
	 * brief	:	Constructor
	 * @param	:	the type of the notifier
	 */
	Notifer(tUInt32 my_udp_port,string remote_host, tUInt32 remote_port, string member_name,tOTType otconn_type, FileLog* filelog = NULL);
	/**
	 * brief	:	Destructor
	 */
	~Notifer();

protected:
	//
	virtual void Execute(Thread::Arg arg);

	tBridgeMsg*	CreateNotificationMsg(OTConn::tOTType);

	tUInt32	_my_udp_port;

};

#endif //__NOTIFIER_HXX__