
// C++ header file "SecUdpNeter.h" for class SecUdpNeter generated by Poseidon for UML.
// Poseidon for UML is developed by Gentleware (http://www.gentleware.com).
// Generated with velocity template engine (http://jakarta.apache.org/velocity).
// Implemention of a secure communication with Udp with acknowledgement.

#ifndef SecUdpNet_I1168f027m12810903affmm7e5f_H
#define SecUdpNet_I1168f027m12810903affmm7e5f_H

#include <list>
#include "mytypes.h"
#include "udpsocket.hxx"
#include "safeque.hxx"
#include "mythread.hxx"
#include "bridgenet.hxx"

class UdpSocket;
class FileLog;

class EXPORT_BRIDGENET SecUdpNeter	:	public Thread
{
public:
//////////////////////////////////TYPEDEFS////////////////////////////////////////////////////
typedef SafeQue<tBridgeMsg*> tBridgeMsgQue;
typedef struct
{
	tBridgeMsg* omsg;
	tUInt32 tick_counter;
}tUnacknowledgedOMsg;
typedef struct
{
	string ip;
	tUInt32 port;
	tUInt32 tid;
}tMsgBasicInfo;
typedef list<tUnacknowledgedOMsg*> tBridgeUnacknowledgedMsgList;
typedef tBridgeUnacknowledgedMsgList::iterator tBridgeUnacknowledgedMsgListIter;
typedef list<tMsgBasicInfo*> tMsgBasicInfoList;
typedef tMsgBasicInfoList::iterator tMsgBasicInfoListIter;
typedef tMsgBasicInfoList::const_iterator tMsgBasicInfoListConstIter;
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
// public constructors
/////////////////////////////////////////////////////

SecUdpNeter(tUInt32 timingoutms,FileLog* filelog);	// For client end
SecUdpNeter(tUInt32 port,tUInt32 timingoutms,FileLog* filelog);	// For server end
SecUdpNeter(UdpSocket* udpsocket,tUInt32 timingoutms,FileLog* filelog);

/////////////////////////////////////////////////////
// public operations
/////////////////////////////////////////////////////

void Shutdown(void);

void SendTo(tBridgeMsg* omsg, string remote_host, tUInt32 remote_port);

tBool Recv(tBridgeMsg** imsg);

tBool AnyOMsgFailed(void) const;

static string PeerLocalAddress(void);

protected:
	//
	virtual void Execute(Thread::Arg arg);

private:
/////////////////////////////////////////////////////
//	private methods
/////////////////////////////////////////////////////
	tBool IsMsgEverReceived(tBridgeMsg* imsg) const;
	void RegisterIMsgAsEverReceived(tBridgeMsg* imsg);
/////////////////////////////////////////////////////
// private attributes
/////////////////////////////////////////////////////

	tBridgeMsgQue _imsg_que;
	 
	tBridgeMsgQue _omsg_que;
	 
	tBridgeUnacknowledgedMsgList _unacknowledged_osmg_list;
	 
	tBridgeMsgQue _failed_omsg_que;

	tMsgBasicInfoList _ever_received_imsg_list;	//contains ever received imsg info,avoiding re-receival.

	UdpSocket* _socket;
 
	tUInt32 _listen_port;

	tUInt32 _timingoutms;

	tBool _is_shutdown_requested;

	FileLog* _filelog;
};

#endif // SecUdpNet_I1168f027m12810903affmm7e5f_H

