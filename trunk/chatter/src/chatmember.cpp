#include "config.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "wfaudioplayer.hxx"
#include "priv/chatmember.hxx"


ChatMember::ChatMember(tUInt32 userid,string username,string host,tUInt32 udp_port,FileLog* filelog)
		:	_host(host),_port(udp_port),_name(username),_id(userid),_isadmin(FALSE),_sockaddr_in(NULL),
			_player(NULL),_filelog(filelog)
{
	_player = new WFAudioPlayer(BITS_PER_SAMPLE,SAMPLES_SEC,BLOCK_NB,BLOCK_SIZE,CHANNEL_NB,0/*_filelog*/);

	if (_player->Init() < 0)
	{
		delete _player;
		_player = NULL;
	}

	_sockaddr_in = CreateSockaddrIn(host,udp_port);

	if (!_sockaddr_in)
	{
		LOGWARNING("ChatMember::ChatMember:failed to create sockaddr_in for host:"<<_host<<"::"<<_port);
	}
}

ChatMember::~ChatMember()
{
	if (_player)
	{
		_player->UnInit();
		delete _player;
		_player = NULL;
	}

	if (_sockaddr_in)
	{
		delete _sockaddr_in;
		_sockaddr_in = NULL;
	}
}
const string& ChatMember::GetMemHost(void) const
{
	return _host;
}

const string& ChatMember::GetMemName(void) const
{
	return _name;
}
const sockaddr_in* ChatMember::GetSockaddr(void) const
{
	return _sockaddr_in;
}

AudioPlayer* ChatMember::GetPlayer(void) const
{
	return _player;
}
tBool ChatMember::AmIAdmin(void) const
{
	return _isadmin;
}

void ChatMember::SetMeAdmin(void)
{
	_isadmin = TRUE;
}
void ChatMember::ResetMeAdmin(void)
{
	_isadmin = FALSE;
}
tUInt32 ChatMember::GetMemId(void) const
{
	return _id;
}
tUInt32 ChatMember::GetMemPort(void) const
{
	return _port;
}
sockaddr_in* ChatMember::CreateSockaddrIn(string host, tUInt32 udp_port)
{
	sockaddr_in* sockaddrin = new sockaddr_in;

	sockaddrin->sin_family = AF_INET;
	sockaddrin->sin_port = htons(udp_port);

	unsigned long inetaddr=inet_addr(host.c_str());

	if (inetaddr==INADDR_NONE)
	{
		  // host isn't an IP address (dot notation) so we have to resolve
		  // the name with DNS.
		  struct hostent* phost = gethostbyname(host.c_str());

		  if (phost != 0)
		  {
			inetaddr = *((unsigned long *)phost->h_addr_list[0]);
		  }
	}

	if (inetaddr)
	{
		memcpy((void *)&(sockaddrin->sin_addr.s_addr), (void *)&inetaddr, sizeof(inetaddr));
	}
	else
	{
		delete sockaddrin;
		sockaddrin = NULL;
	}

	return sockaddrin;
}