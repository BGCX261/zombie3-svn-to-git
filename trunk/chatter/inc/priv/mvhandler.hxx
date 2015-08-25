#ifndef __MV_HANDLER_HXX__
#define __MV_HANDLER_HXX__

#include <map>
#include "mytypes.h"
#include "mythread.hxx"

class Chatter;
class GroupMgr;
class UdpSocket;
class FileLog;

class MVHandler:public Thread
{
public:
	//
	MVHandler(Chatter* chatter,tUInt32 myid,string centrals_host,tUInt32 centrals_udp_port,UdpSocket* udpsocket,GroupMgr* groupmgr,FileLog* filelog=NULL);

	~MVHandler();

	void Shutdown(void);

	virtual void Execute(Thread::Arg arg);

	/**
	 * @brief:tell the CentralS my voice endpoints,both of localhost and my NAT.
	 */
	tBool RegisterUdpEndpoints(void);

private:
	//
	/**
	 * @brief:since the mapping from local endpoint to NAT endpoint get expired after a span of silence.
	 *  the length of the span varies due to different types of NAT,the shortest span I know is 10 seconds,
	 *  in order to keep the mapping valid,renew it every 10 minutes.this method is to send a senseless data out
	 *  from local endpoint through NAT's endpoint to anywhere,to renew the mapping.
	 */
	void RenewEndpointsMapping(void);
	//
	Chatter*	_chatter;
	tUInt32		_myid;
	string		_centrals_host;
	tUInt32		_centrals_cport;
	UdpSocket*	_udpsocket;
	GroupMgr*	_groupmgr;
	tBool		_is_shutdown_requested;
	FileLog*	_filelog;
};
#endif  //__MV_HANDLER_HXX__