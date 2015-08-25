#include "priv/cacherdb.hxx"

CacherDB* CacherDB::_instance = NULL;

CacherDB::CacherDB()
	:	_id_info_map()
{}

CacherDB::~CacherDB()
{}
CacherDB* CacherDB::GetInstance(void)
{
	if (!_instance)
		_instance = new CacherDB();

	return _instance;
}
void CacherDB::LoginUser(tUInt32 id,string name,string public_ip,tUInt32 public_st_udp_port,string internal_ip)
{
	/* before insertion,clean the stale.*/
	tIdInfoIter it = _id_info_map.find(id);

	if (it!=_id_info_map.end())
	{
		delete (it->second);
		_id_info_map.erase(it);
	}

	tUserInfo* userinfo = new tUserInfo();

	userinfo->id = id;
	userinfo->name = name;
	userinfo->public_ip	= public_ip;
	userinfo->public_st_udp_port = public_st_udp_port;
	userinfo->internal_ip = internal_ip;
	userinfo->user_state = USER_STATE_ONLINE;

	_id_info_map.insert(tIdInfoPair(id,userinfo));
}
void CacherDB::LogoffUser(tUInt32 id)
{
	// not implemenmted
}
tBool CacherDB::SetPTUdpEndpoint(tUInt32 id,tUInt32 public_pt_udp_port,tUInt32 internal_pt_udp_port)
{
	tIdInfoIter it = _id_info_map.find(id);

	if (it==_id_info_map.end())
		return FALSE;

	it->second->public_pt_udp_port = public_pt_udp_port;
	it->second->internal_pt_udp_port = internal_pt_udp_port;

	return TRUE;
}
tBool CacherDB::SetPTUdpEndpoint2(tUInt32 id,tUInt32 public_pt_udp_port2,tUInt32 internal_pt_udp_port2)
{
	tIdInfoIter it = _id_info_map.find(id);

	if (it==_id_info_map.end())
		return FALSE;

	it->second->public_pt_udp_port2 = public_pt_udp_port2;
	it->second->internal_pt_udp_port2 = internal_pt_udp_port2;

	return TRUE;
}
tBool CacherDB::SetPTTcpEndpoint(tUInt32 id,tUInt32 public_pt_tcp_port,tUInt32 internal_pt_tcp_port)
{
	tIdInfoIter it = _id_info_map.find(id);

	if (it==_id_info_map.end())
		return FALSE;

	it->second->public_pt_tcp_port = public_pt_tcp_port;
	it->second->internal_pt_tcp_port = internal_pt_tcp_port;

	return TRUE;
}
tUserInfo const * CacherDB::GetUserInfo(tUInt32 id)
{
	tIdInfoIter it = _id_info_map.find(id);

	if (it==_id_info_map.end())
		return NULL;
	else
		return it->second;
}