#ifndef __BRIDGE_MESSAGE_HXX__
#define __BRIDGE_MESSAGE_HXX__

#include "mytypes.h"
#include "was_api.h"

#ifdef WIN32

#ifdef BRIDGENET_EXPORTS

/* DLL export */
#ifdef EXPORT_BRIDGENET
#undef EXPORT_BRIDGENET
#endif

#define EXPORT_BRIDGENET __declspec(dllexport)
#else
/* EXE import */
#define EXPORT_BRIDGENET __declspec(dllimport)
//#define EXPORT
#endif

#endif

#define HEADERSIZE	24
#define	TAILSIZE	4

typedef struct tagBridgeMsg
{
	/** For Udp to determine the remote endpoint.*/
	string rhost;
	tUInt32 rport;

	tInt32	mpi;
	tInt32	tom;
	tInt32	tid;
	tInt32	rspc;
	tInt32	lom;
	tByte*	msg;

	tBool	isReceived;
}tBridgeMsg;

EXPORT_BRIDGENET tBridgeMsg* CreateBridgeMsg(void);
EXPORT_BRIDGENET void ReleaseBridgeMsg(tBridgeMsg** msg);

/** Create a copy of the msg */
EXPORT_BRIDGENET tBridgeMsg* DuplicateMsg(tBridgeMsg* msg);

EXPORT_BRIDGENET tChar* GetDataFromMsg(tBridgeMsg* msg,E_WAS_TAG_ID tag_id);
EXPORT_BRIDGENET void ReleaseDataFromMsg(tChar* pdata);

#endif