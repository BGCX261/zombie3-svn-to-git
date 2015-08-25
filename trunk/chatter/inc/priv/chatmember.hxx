#ifndef __CHAT_MEMBER_HXX__
#define __CHAT_MEMBER_HXX__

/**
 * ChatMember represents a chatting member, keeping all info relevant to it
 */
#include <winsock2.h>	// sockaddr_in
#include "mydll.h"
#include "mytypes.h"
#include "taskdefs.h"

class AudioPlayer;
class TcpSocket;
class FileLog;

class ChatMember
{
public:
	/**
	 * brief	:	constructor
	 * @param	:	sockaddr_in:the udp sockaddr created with the host and the certain udp port for convinence to access
	 */
	ChatMember(tUInt32 userid,string username,string host,tUInt32 udp_port,FileLog* filelog);

	/**
	 * brief	:	destructor
	 */
	~ChatMember();

	tUInt32		GetMemId(void) const;
	tUInt32		GetMemPort(void) const;

	const string&		GetMemHost(void) const;
	const string&		GetMemName(void) const;
	const sockaddr_in*	GetSockaddr(void) const;

	AudioPlayer*		GetPlayer(void) const;

	tBool AmIAdmin(void) const;
	void SetMeAdmin(void);
	void ResetMeAdmin(void);

private:
	/**
	 * @brief	:	Create the sockaddr_in struct for a given host and port
	 * @return	:	If successful returns the pointer to the object created, or NULL
	 */
	sockaddr_in* CreateSockaddrIn(string host, tUInt32 port);

	tUInt32 _id;
	string _name;
	tBool _isadmin;
	
	string	_host;
	tUInt32 _port;
	/**
	 * The udp sockaddr created with the _host and _port for the convinence to send voice data to.
	 */
	sockaddr_in* _sockaddr_in;

	/**
	 * The player to play back the voice data from the member
	 */
	AudioPlayer* _player;

	FileLog* _filelog;
};

#endif //__CHAT_MEMBER_HXX__