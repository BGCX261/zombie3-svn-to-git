#include "priv/notifier.hxx"
#include "was_api.h"
#include "bridgemessage.hxx"
#include "bridgenet.hxx"

Notifer::Notifer(tUInt32 my_udp_port, string remote_host, tUInt32 remote_port, string member_name,tOTType otconn_type, FileLog* filelog)
	:	OTConn(remote_host,remote_port,member_name,otconn_type,filelog),_my_udp_port(my_udp_port)
{}

Notifer::~Notifer()
{}

void Notifer::Execute(Thread::Arg arg)
{
	/**
	 * 1) create the notification message
	 * 2) sendd it to the destinate member
	 * 3) wait for the confirm
	 * 4) change the state of the otconn
	 * 5) end
	 */
}

tBridgeMsg*	Notifer::CreateAddMemMsg()
{
	tUInt32 len;

	tChar port_str[10] = {0};
	sprintf(port_str, "%s", _my_udp_port);

	tByte* msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_INVITEPEER_REQ,1, X_WAS_RSPC_NONE, E_WAS_TAG_ID_UPPT, 
								strlen(port_str), port_str, 
								E_WAS_TAG_ID_NONE);

	if (!msg)
		return NULL;

	tBridgeMsg* bridgemsg = new tBridgeMsg();

	bridgemsg->lom = len;
	bridgemsg->mpi = E_WAS_MSG_FIRST_PART;
	bridgemsg->tid = 1;
	bridgemsg->rspc= X_WAS_RSPC_NONE;
	bridgemsg->msg = msg;

	switch (ottype)
	{
	case INVITE_MEMBER_TYPE:
		bridgemsg->tom = E_WAS_MSG_INVITEPEER_REQ;
		break;
	case ADD_MEMBER_TYPE:
		bridgemsg->tom = E_WAS_MSG_ADDMEMBER_REQ;
		was_append_to_msg(&bridgemsg->lom, bridgemsg->msg, E_WAS_TAG_ID_UPPT
		break;
	}

	return bridgemsg;
}