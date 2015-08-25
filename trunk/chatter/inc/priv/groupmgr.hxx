#ifndef __GROUP_MGR_HXX__
#define __GROUP_MGR_HXX__

#include <map>
#include <vector>

#include "mytypes.h"
#include "priv/chatmember.hxx"

struct sockaddr_in;
class FileLog;

class GroupMgr
{
public:
	////////////////////////////////////TYPEDEFS////////////////////////////////////
	typedef enum
	{
		GROUP_TYPE_UNDEFINED,
		GROUP_TYPE_VOICE,
		GROUP_TYPE_VIDEO,
		GROUP_TYPE_NUM
	}tGroupType;

	typedef map<string,ChatMember*> tHostChatMemberMap;
	typedef tHostChatMemberMap::iterator tHostChatMemberMapIter;
	typedef tHostChatMemberMap::const_iterator tHostChatMemberMapConstIter;
	typedef pair<string,ChatMember*> tHostChatMemberPair;

	typedef struct
	{
		tUInt32 userid;
		string	username;
		tBool	isadmin;
		string	public_ip;
		string	internal_ip;
		tUInt32 public_udp_port;
		tUInt32 internal_udp_port;
	}tDoubleEndpoints;

	typedef vector<tDoubleEndpoints*>	tDoubleEndsVec;
	typedef tDoubleEndsVec::iterator	tDoubleEndsVecIter;
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief	:	constructor
	 */
	GroupMgr(FileLog* filelog=NULL);

	/**
	 * @brief	:	destructor
	 */
	~GroupMgr();

	tBool IsEmpty(void) const;

	/**
	 * @brief	:	add a member with both its public endpoint and internal endpoint,waiting for the endpoint to be checked by MVHandler.
	 */
	void AddMember2Add(tUInt32 userid,string username,string public_ip,tUInt32 public_udp_port,string internal_ip,tUInt32 internal_udp_port);

	tBool AnyMember2Add(void) const;

	tInt32 Member2AddNum(void) const;

	tDoubleEndpoints const* GetDoubleEndpointsAt(tUInt32 index) const;

	/**
	 * @brief	:	Check Member2Add list see if any endpoint match the endpoint specified by the ip and port,if does add the member.
	 * @param ip
	 * @param port
	 */
	tBool Try2AddMember2Add(string ip,tUInt32 port,tUInt32& userid,string& username);

	tBool DeleteMemberById(tUInt32 userid);

	ChatMember const* GetChatMemberById(tUInt32 userid) const;

	tHostChatMemberMap const& GetHostChatMemberMap(void) const;

	void SetAllPlayerCurPos(void);
	tBool SetVoiceDataByHost(string ip,tByte*data,tUInt32 size);

	tBool AmIAdmin(void) const;
	void SetMeAdmin(void);
	void ResetMeAdmin(void);

	tBool SetUserAdmin(tUInt32 userid);

	tGroupType GetGroupType(void) const;
	void SetGroupType(tGroupType gtype);

private:
/////////////////////////////////////////////////////////Private methods/////////////////////////////////////////////////////////////////////
	void AddMember(tUInt32 userid,string mem_name,string host,tUInt32 port);
	void ResetAllAdmin(void);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	tDoubleEndsVec _doubleends_vec;
	tHostChatMemberMap _host_member_map;

	tBool _am_I_admin;
	tGroupType _group_type;

	FileLog* _filelog;
};

#endif //__GROUP_MGR_HXX__