/************************************************************************************
 * The cache DB(or currently in memory) to store logined users' info, including:	*
 * userid,username,public_tcp_address,internal_tcp_address,public_udp_address		*
 * internal_udp_address																*
 ************************************************************************************/

#ifndef __CACHER_DB_HXX__
#define __CACHER_DB_HXX__

#include <map>
#include "mytypes.h"

typedef enum
{
	USER_STATE_UNDEFINED,
	USER_STATE_OFFLINE,
	USER_STATE_ONLINE,
	USER_STATE_HIDDEN,
	USER_STATE_BUSY,
	USER_STATE_MUTE
}tUserState;
typedef struct
{
	tUInt32 id;
	string name;
	string public_ip;
	string internal_ip;

	/**for communication with CentralS.*/
	tUInt32 public_st_udp_port;	
	
	/**for Tcp communication with peer.*/
	tUInt32 public_pt_tcp_port;
	tUInt32 internal_pt_tcp_port;
	
	/**for voice Udp communication with peer.*/
	tUInt32 public_pt_udp_port;
	tUInt32 internal_pt_udp_port;
	/**for video Udp communication with peer.*/
	tUInt32 public_pt_udp_port2;
	tUInt32 internal_pt_udp_port2;

	tUserState user_state;
}tUserInfo;


/** Temporily make it a singleton.*/
class CacherDB
{
public:
	////////////////////////////////TYPEDEFS//////////////////////////////////////////
	typedef map<tUInt32,tUserInfo*>		tIdInfoMap;
	typedef tIdInfoMap::iterator		tIdInfoIter;
	typedef tIdInfoMap::const_iterator	tIdInfoConstIter;
	typedef pair<tUInt32,tUserInfo*>	tIdInfoPair;
	//////////////////////////////////////////////////////////////////////////////////

	CacherDB();

	virtual ~CacherDB();

	static CacherDB* GetInstance(void);

	void	LoginUser(tUInt32 id,string name,string public_ip,tUInt32 public_st_udp_port,string internal_ip);
	void	LogoffUser(tUInt32 id);

	tBool	SetPTUdpEndpoint(tUInt32 id,tUInt32 public_pt_udp_port,tUInt32 internal_pt_udp_port);
	tBool	SetPTUdpEndpoint2(tUInt32 id,tUInt32 public_pt_udp_port2,tUInt32 internal_pt_udp_port2);
	tBool	SetPTTcpEndpoint(tUInt32 id,tUInt32 public_pt_tcp_port,tUInt32 internal_pt_tcp_port);

	tUserInfo const * GetUserInfo(tUInt32 id);

private:
	//
	static CacherDB* _instance;

	/**
	 * For storing logined userinfo, later it will be substituted for by DB.*/
	tIdInfoMap _id_info_map;
};



#endif	//__CACHER_DB_HXX__