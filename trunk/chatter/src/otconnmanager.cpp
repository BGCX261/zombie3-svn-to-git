#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "priv/otconnmanager.hxx"

OTConnManager::OTConnManager(FileLog* filelog)
			:	_userid_otconn_map(),
				_filelog(filelog)
{}
OTConnManager::~OTConnManager()
{}
tUInt32 OTConnManager::Transfer(tUInt32 userid,string file_path_and_name)
{
	//LOGDEBUG("OTConnManager::Transfer:requesting to transfer peer file '"<<file_path_and_name<<" to "<<remote_host<<".");

	//TConn*	otconn = TheOTConn2Server(remote_host,remote_port);

	//if (!otconn)
	//	return 0;
	//else
	//	return otconn->Transfer(file_path_and_name);

	return 0;
}
TConn* OTConnManager::GetOTConn(tUInt32 userid) const
{
	tUserIdOTConnMapConstIter iter = _userid_otconn_map.find(userid);

	if (iter==_userid_otconn_map.end())
		return NULL;
	else
		return iter->second;
}
