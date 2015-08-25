#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "audioplayer.hxx"
#include "priv/chatmember.hxx"
#include "priv/groupmgr.hxx"


GroupMgr::GroupMgr(FileLog* filelog)
		:	_doubleends_vec(),
			_host_member_map(),
			_am_I_admin(FALSE),
			_group_type(GROUP_TYPE_UNDEFINED),
			_filelog(filelog)
{}
GroupMgr::~GroupMgr()
{}
tBool GroupMgr::IsEmpty(void) const
{
	return (_doubleends_vec.empty() && _host_member_map.empty());
}
void GroupMgr::AddMember2Add(tUInt32 userid,string username,string public_ip,tUInt32 public_udp_port,string internal_ip,tUInt32 internal_udp_port)
{
	/** Register it to be added.*/
	tDoubleEndpoints* doubleends = new tDoubleEndpoints();

	doubleends->userid = userid;
	doubleends->username = username;
	doubleends->public_ip = public_ip;
	doubleends->internal_ip = internal_ip;
	doubleends->public_udp_port = public_udp_port;
	doubleends->internal_udp_port = internal_udp_port;

	_doubleends_vec.push_back(doubleends);

	LOGDEBUG("GroupMgr::AddMember2Add:Added member2add \nid\t"<<userid<<"\nname\t"<<username<<"\npublic endpoint\t"<<doubleends->public_ip<<"::"<<doubleends->public_udp_port
					<<"\ninternal endpoint\t"<<doubleends->internal_ip<<"::"<<doubleends->internal_udp_port);
}
tBool GroupMgr::AnyMember2Add(void) const
{
	return !_doubleends_vec.empty();
}
tInt32 GroupMgr::Member2AddNum(void) const
{
	return static_cast<tInt32>(_doubleends_vec.size());
}
GroupMgr::tDoubleEndpoints const* GroupMgr::GetDoubleEndpointsAt(tUInt32 index) const
{
	return _doubleends_vec[index];
}
tBool GroupMgr::Try2AddMember2Add(string ip,tUInt32 port,tUInt32& userid,string& username)
{
	if (_doubleends_vec.empty())
		return FALSE;

	tBool ret = FALSE;

	tDoubleEndpoints* doubleends;
	tDoubleEndsVecIter it = _doubleends_vec.begin();

	while (it!=_doubleends_vec.end())
	{
		doubleends = (*it);

		if (doubleends->public_ip.compare(ip)==0 || doubleends->internal_ip.compare(ip)==0)
		{
			ret = TRUE;
			userid = doubleends->userid;
			username = doubleends->username;
			AddMember(doubleends->userid,doubleends->username,ip,port);
			delete doubleends;
			_doubleends_vec.erase(it);
			break;
		}
		else
			it++;
	}

	return ret;
}
void GroupMgr::AddMember(tUInt32 userid,string username,string host,tUInt32 port)
{
	/**delete if if it already exists.*/
	DeleteMemberById(userid);

	ChatMember* member = new ChatMember(userid,username,host,port,_filelog);

	_host_member_map.insert(tHostChatMemberPair(host,member));

	LOGDEBUG("GroupMgr::AddMember:Added member '"<<username<<" #"<<userid<<" at '"<<host<<"::"<<port<<"'.");
}

tBool GroupMgr::DeleteMemberById(tUInt32 userid)
{
	tBool ret = FALSE;
	tHostChatMemberMapIter it;

	for (it=_host_member_map.begin(); it!=_host_member_map.end(); )
	{
		if (it->second->GetMemId()==userid)
		{
			delete it->second;
			_host_member_map.erase(it++);
			ret = TRUE;
		}
		else
			it++;
	}

	return ret;
}
ChatMember const* GroupMgr::GetChatMemberById(tUInt32 userid) const
{
	tHostChatMemberMapConstIter it=_host_member_map.begin();

	while (it!=_host_member_map.end())
	{
		if (it->second->GetMemId()==userid)
			return it->second;
		else
			it++;
	}

	return NULL;
}
GroupMgr::tHostChatMemberMap const&  GroupMgr::GetHostChatMemberMap(void) const
{
	return _host_member_map;
}
void GroupMgr::SetAllPlayerCurPos(void)
{
	tHostChatMemberMapIter it = _host_member_map.begin();

	while (it != _host_member_map.end())
	{
		it->second->GetPlayer()->SetCurrentPos();

		it++;
	}
}
tBool GroupMgr::SetVoiceDataByHost(string ip,tByte*data,tUInt32 size)
{
	tHostChatMemberMapIter it = _host_member_map.find(ip);

	if (it!=_host_member_map.end())
		return (it->second->GetPlayer()->WriteAudio(data,size) > 0);
	else
	{
		LOGWARNING("GroupMgr::SetVoiceDataByHost:No member is from host '"<<ip<<"'.");
		return FALSE;
	}
}

tBool GroupMgr::AmIAdmin(void) const
{
	return _am_I_admin;
}
void GroupMgr::SetMeAdmin(void)
{
	ResetAllAdmin();

	_am_I_admin=TRUE;
}
void GroupMgr::ResetMeAdmin(void)
{
	_am_I_admin=FALSE;
}
tBool GroupMgr::SetUserAdmin(tUInt32 userid)
{
	tDoubleEndpoints* pde=NULL;
	ChatMember* pcm=NULL;

	tDoubleEndsVecIter iter = _doubleends_vec.begin();
	while (iter!=_doubleends_vec.end())
	{
		if ((*iter)->userid==userid)
		{
			pde = (*iter);
			break;
		}
		else 
			++iter;
	}

	if (!pde)
	{
		tHostChatMemberMapIter iter2 = _host_member_map.begin();
		while (iter2!=_host_member_map.end())
		{
			if (iter2->second->GetMemId()==userid)
			{
				pcm = iter2->second;
				break;
			}
			else 
				++iter2;
		}
	}

	if (!pde && !pcm)
		return FALSE;

	ResetAllAdmin();

	if (pde) pde->isadmin = TRUE;
	if (pcm) pcm->SetMeAdmin();

	return TRUE;
}
GroupMgr::tGroupType GroupMgr::GetGroupType(void) const
{
	return _group_type;
}
void GroupMgr::SetGroupType(tGroupType gtype)
{
	_group_type = gtype;
}
void GroupMgr::ResetAllAdmin(void)
{
	ResetMeAdmin();

	tDoubleEndsVecIter iter = _doubleends_vec.begin();

	while (iter !=_doubleends_vec.end())
	{
		(*iter)->isadmin = FALSE;
		++iter;
	}

	tHostChatMemberMapIter iter2 = _host_member_map.begin();

	while (iter2!=_host_member_map.end())
	{
		iter2->second->ResetMeAdmin();
		++iter2;
	}
}