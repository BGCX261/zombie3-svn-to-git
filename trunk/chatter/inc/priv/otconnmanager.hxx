/********************************************************************************************
 * Managing out going Task connections,normally there are two types of connections:			*
 * 1)connection to Central Server.															*
 * 2)connection to peer.																	*
 * currently connection (2) doesn't exists, all task sis done via connection to central		*
 * server,including transmission.															*
 *******************************************************************************************/

#ifndef __OTCONN_MANAGER_HXX__
#define __OTCONN_MANAGER_HXX__

#include <map>
#include "mytypes.h"
#include "taskdefs.h"

class TConn;
class FileLog;

class OTConnManager
{
public:
	/**
	 * Define the map from connid to the connection,normally the connid is the UserId
	 * except for connection to Central Server.
	 */
	typedef map<tUInt32,TConn*> tUserIdOTConnMap;
	typedef map<tUInt32,TConn*>::iterator tUserIdOTConnMapIter;
	typedef map<tUInt32,TConn*>::const_iterator tUserIdOTConnMapConstIter;
	typedef pair<tUInt32,TConn*> tUserIdOTConnPair;

	/**
	 * @brief	:	Constructor
	 * @param chatter	:	pointer to current chatter object
	 * @param filelog	:
	 */
	OTConnManager(FileLog* filelog = NULL);

	//@brief	:	Destructor
	virtual ~OTConnManager();

	/**
	 * @brief	:	transfer file(s) to specific destination
	 * @param userid	:	the user to transmit file to
	 * @param file_path_and_name
	 * @return	:	the task id
	 */
	tUInt32 Transfer(tUInt32 userid,string file_path_and_name);

private:

	TConn* GetOTConn(tUInt32 userid) const;

	//
	tUserIdOTConnMap _userid_otconn_map;

	FileLog* _filelog;
};

#endif //__OTCONN_MANAGER_HXX__