#include "config.hxx"
#include "taskdefs.h"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "wfaudiorecorder.hxx"
#include "udpsocketexception.hxx"
#include "udpsocket.hxx"
#include "secudpneter.hxx"
#include "configfile.hxx"
#include "priv/groupmgr.hxx"
#include "priv/tstatusmgr.hxx"
#include "priv/recordhandler.hxx"
#include "priv/mvhandler.hxx"
#include "priv/stconn.hxx"
#include "chatter.hxx"

#define CONCAT3(a,b,c)     a##b.c
#define MAKE_STRING1(a)  #a
#define MAKE_STRING(a)  MAKE_STRING1(a)

#define CONFIG_FILE1(x)    CONCAT3(centrals_,x,conf)
#define CONFIG_FILE(x)     CONFIG_FILE1(x)


////////////////////////////////////////////////////////
// configurations
////////////////////////////////////////////////////////
static ConfigFile _configfile;
static string centrals_host;
static tSInt32 centrals_cport;
static tSInt32 debug;
static bool ReadConfigFile()
{
  int errcount;
  const char **str_param;
  const char *str_param_list[]=
    {
      "CentralSHost",
#if defined(linux)
      "User",
      "Group",
#endif
      0
    };

  string **str_param_strg;
  string *str_param_strg_list[]=
    {
      &centrals_host,
#if defined(linux)
      &username,
      &groupname,
#endif
      0
    };

  const char **int_param;
  const char *int_param_list[]=
    {
      "CentralSCPort",
	  "Debug",
      0
    };

  tSInt32 **int_param_strg;
  tSInt32 *int_param_strg_list[]=
    {
      &centrals_cport,
	  &debug,
      0
    };

  errcount=0;

  // We accept 3 (for Linux) or 2 (for Windows) different locations for the configuration file.
  string path1=string("/etc/"); path1.append(string(MAKE_STRING(CONFIG_FILE(SERVERTYPE))));
  string path2=string("etc/"); path2.append(string(MAKE_STRING(CONFIG_FILE(SERVERTYPE))));
  string path3=string("./"); path3.append(string(MAKE_STRING(CONFIG_FILE(SERVERTYPE))));

  // Reading configuration file 
#if defined(linux)
  if (!_configfile.Read(path3) && !_configfile.Read(path2) && !_configfile.Read(path1))
#elif defined(WIN32)
  //if (!_configfile.Read(path3) && !_configfile.Read(path3))
#else
#error "Unsupported OS"
  if (false)
#endif
    //{
      //cerr<<"[ERROR] Failed to read config file.\n";
      //return FALSE;
    //}

  // Assign all string variables one by one. If one is missing, then it is reported
  // and will lead to error.
  for (str_param=&str_param_list[0],
	 str_param_strg=&str_param_strg_list[0]; 
       (*str_param!=0) && (*str_param_strg!=0); 
       str_param++,str_param_strg++)
    {
      if (!_configfile.GetValue(*str_param,**str_param_strg))
	{
	  cerr<<"[ERROR] Missing '"<<*str_param<<"' parameter in config file.\n";
	  errcount++;
	}      
    }
  
  // Assign all integer variables one by one. If one is missing, then it is reported
  // and will lead to error.
  for (int_param=&int_param_list[0],
	 int_param_strg=&int_param_strg_list[0]; 
       (*int_param!=0) && (*int_param_strg!=0); 
       int_param++,int_param_strg++)
    {
      if (!_configfile.GetValue(*int_param,*int_param_strg))
	  {
	    cerr<<"[ERROR] Missing '"<<*int_param<<"' parameter in config file.\n";
	    errcount++;
	  }      
    }

  // Successful only if no error encountered.
  return errcount==0;
}

string tagTStatus::msgtext()
{
	if (tasktype!=TASK_TYPE_INSTANT_MSG_OUT && tasktype!=TASK_TYPE_INSTANT_MSG)
		return "";

	return reinterpret_cast<char*>(arg1);
}
string tagTStatus::tittle()
{
	string tittle;

	switch (tasktype)
	{
	case TASK_TYPE_JOIN_VOICE_CHAT:
		tittle = username+" 请求您语音聊天...";
		break;
	case TASK_TYPE_ADD_VOICE_GROUP_CHAT:
		tittle = username+" 邀请您加入语音聊天...";
		break;
	case TASK_TYPE_INVITE_VEDIO_CHAT:
		break;
	case TASK_TYPE_TRANSFER_FILE:
		tittle = "向";
		tittle += username+"传送文件...\n";
		tittle += reinterpret_cast<char*>(arg1);
		break;
	case TASK_TYPE_JOIN_VOICE_CHAT_OUT:
		tittle = "请求"+username+"语音聊天...";
		break;
	case TASK_TYPE_ADD_VOICE_GROUP_CHAT_OUT:
		tittle = "邀请"+username+"加入语音聊天...";
		break;
	case TASK_TYPE_INVITE_VEDIO_CHAT_OUT:
	case TASK_TYPE_TRANSFER_FILE_OUT:
		tittle = "向";
		tittle += username+"传送文件...\n";
		tittle += reinterpret_cast<char*>(arg1);
		break;
	default:
		tittle="no tittle";
		break;
	}

	return tittle;
}
string tagTStatus::notification()
{
	string notification;

	if (isin())
		notification = "您";
	else
		notification = username;

	if (tstate==TASK_STATE_REQUEST_CONFIRMED)
		notification += "同意了";
	else if (tstate==TASK_STATE_REQUEST_REJECTED)
		notification += "拒绝了";
	else if (tstate==TASK_STATE_CANCELLED)
		notification += "取消了";
	else if (tstate==TASK_STATE_TERMINATED)
		notification += "终止了";
	else if (tstate==TASK_STATE_REQUEST_IGNORED_FOR_BUSY)
		notification += "因为已在会话中,忽略了";
	else return "";

	if (isin())
		notification += username+"的";
	else
		notification += "您的";

	switch (tasktype)
	{
	case TASK_TYPE_JOIN_VOICE_CHAT:
	case TASK_TYPE_ADD_VOICE_GROUP_CHAT:
	case TASK_TYPE_JOIN_VOICE_CHAT_OUT:
	case TASK_TYPE_ADD_VOICE_GROUP_CHAT_OUT:
		notification += "语音聊天请求。";
		break;
	case TASK_TYPE_INVITE_VEDIO_CHAT:
	case TASK_TYPE_INVITE_VEDIO_CHAT_OUT:
		notification += "视频聊天请求。";
		break;
	default:
		notification="";
		break;
	}

	return notification;
}
bool tagTStatus::isin()
{
	return (tasktype>= TASK_TYPE_INSTANT_MSG && tasktype<=TASK_TYPE_TRANSFER_FILE);
}

float tagTStatus::progress()
{
	if (total==0)
		return 0;
	else
		return ((float)finished/(float)total);
}

Chatter::Chatter(tUpdateFunc update_func,tUInt32 myid,char* myname):
		_groupmgr(NULL),
		_tstatusmgr(NULL),
		_stconn(NULL),
		_recorder(NULL),
		_vudpsocket(NULL),
		_mvhandler(NULL),
		_recordhandler(NULL),
		_update_func(update_func),
		_chatter_state(CHATTER_STATE_UNKNOWN),
		_myid(myid),
		_myname(myname),
		_is_building_session(FALSE),
		_is_shutdown_requested(FALSE),
		_filelog(NULL)
{
}

Chatter::~Chatter()
{
}
void Chatter::Shutdown(void)
{
	_is_shutdown_requested = TRUE;
}
char const * Chatter::GetMyName(void) const
{
	return _myname.c_str();
}
tUInt32 Chatter::GetMyId(void) const
{
	return _myid;
}
Chatter::tChatterState Chatter::ChatterState(void) const
{
	return _chatter_state;
}
tBool Chatter::EnsureVoiceOpen(void)
{
	if (_mvhandler && _recordhandler)
		return TRUE;

	tBool res = TRUE;

	LOGDEBUG("RecordHandler::EnsureVoiceOpen:Started...");

	try
	{
		_vudpsocket = new UdpSocket();
	}
	catch (UdpSocketException &e)
	{
		LOGERROR("RecordHandler::EnsureVoiceOpen:Failed to instantiate a UdpSocket:"<<e.Description()<<",shutting down...");
		
		if (_vudpsocket)
			delete _vudpsocket;

		return FALSE;
	}

	_mvhandler = new MVHandler(this,_myid,centrals_host,centrals_cport,_vudpsocket,_groupmgr,_filelog);

	if (!_mvhandler->RegisterUdpEndpoints())
	{
		delete _mvhandler;
		_mvhandler = NULL;
		return FALSE;
	}

	LOGINFO("Chatter::EnsureVoiceOpen:Registering MV udp endpoints succeeded.");

	_mvhandler->Start();

	_recorder = new WFAudioRecorder(BITS_PER_SAMPLE,SAMPLES_SEC,BLOCK_NB,BLOCK_SIZE,CHANNEL_NB,0,0/*_filelog*/);
	_recordhandler = new RecordHandler(_recorder,_vudpsocket,_groupmgr,0/*_filelog*/);
	_recordhandler->Start();
	_recorder->SetCallBackThreadId(_recordhandler->RealThreadId());
	_recorder->Start();

	LOGDEBUG("RecordHandler::EnsureVoiceOpen:Ended.");

	return TRUE;
}
void Chatter::CloseVoice(void)
{
	if (_mvhandler)		_mvhandler->Shutdown();
	if (_recorder)		_recorder->Shutdown();
	if (_recordhandler)	_recordhandler->Shutdown();

	tUInt32 livecounter=4;
	tUInt32 counter = 0;
	while (counter != 0 && counter++ < 10)
	{
		if(_mvhandler && Thread::FINISHED_STATE == _mvhandler->State())
		{
			livecounter--;
			_mvhandler->Join();
			delete _mvhandler;
			_mvhandler = NULL;
		}
		if(_recorder && Thread::FINISHED_STATE == _recorder->State()) 
		{
			livecounter--;
			_recorder->Join();
			_recorder->UnInit();
			delete _recorder;
			_recorder = NULL;
		}
		if(_recordhandler && Thread::FINISHED_STATE == _recordhandler->State())
		{
			livecounter--;
			_recordhandler->Join();
			_recordhandler->UnInit();
			delete _recordhandler;
			_recordhandler = NULL;
		}
	}

	if (livecounter)
		LOGWARNING("Chatter::Execute:Some thread(s) won't end,but sorry we will not join any longer...");

	if (_vudpsocket)
	{
		_vudpsocket->Close();
		delete _vudpsocket;
	}

	_mvhandler = NULL;
	_recorder = NULL;
	_recordhandler = NULL;
	_vudpsocket = NULL;
}
tUInt32 Chatter::SendInstantMsg(tUInt32 userid,char* username,char* instantmsg)
{
	if (!_stconn)
		return 0;
	else	//encode it before sending,TO DO
		return _stconn->SendInstantMsg(userid,username,_myid,_myname,instantmsg);
}
tUInt32 Chatter::InvitePeerVoiceChat(tUInt32 userid,char* username)
{
	if (!_stconn)
		return 0;

	string reason;

	if (!EnsureVoiceOpen())
	{
		reason = "注册语音端口失败!";

		tChar* pmsg = new tChar[reason.length()+1];
		strcpy(pmsg,reason.data());
		pmsg[reason.length()]=0;
		_tstatusmgr->RegisterOutTStatus(userid,username,0,TASK_TYPE_JOIN_VOICE_CHAT,TASK_STATE_MY_ERROR,static_cast<void*>(pmsg));
		return 0;
	}
	else if (!CanIInviteVoice(reason))
	{
		tChar* pmsg = new tChar[reason.length()+1];
		strcpy(pmsg,reason.data());
		pmsg[reason.length()]=0;
		_tstatusmgr->RegisterOutTStatus(userid,username,0,TASK_TYPE_JOIN_VOICE_CHAT,TASK_STATE_REQUEST_IGNORED_FOR_BUSY,static_cast<void*>(pmsg));
		return 0;
	}
	else
	{
		if (AmIAdmin())
			return _stconn->AddVoiceGroupChat(userid,username,_myid,_myname);
		else
			return _stconn->JoinVoiceChat(userid,username,_myid,_myname);
	}
}
tUInt32 Chatter::InvitePeerVideoChat(tUInt32 userid,char* username)
{
	if (!_stconn)
		return 0;
	else
		return _stconn->InvitePeerVideoChat(userid,username,_myid,_myname);
}
tUInt32 Chatter::Transfer(tUInt32 userid,char* username,char* file_path_and_name)
{
	//if (!_stconn)
	//	return 0;
	//else
	//	return _stconn->Transfer(userid,username,_myid,_myname,file_path_and_name);
	return 0;
}
tTStatus* Chatter::QueryTStatus(tUInt32 tstatusid)
{
	if (!_tstatusmgr)
		return FALSE;

	return _tstatusmgr->QueryTStatus(tstatusid);
}
void Chatter::AcceptRequest(tUInt32 tstatusid,char* additional)
{
	if (!_stconn)
		return ;

	tTStatus* tstatus;

	if (!(tstatus=_tstatusmgr->QueryTStatus(tstatusid)))
		return ;
	
	_stconn->AcceptRequest(tstatus->userid,tstatus->tid,_myid,additional);
}
void Chatter::RejectRequest(tUInt32 tstatusid)
{
	if (!_stconn)
		return ;

	tTStatus* tstatus;

	if (!(tstatus=_tstatusmgr->QueryTStatus(tstatusid)))
		return ;
	
	_stconn->RejectRequest(tstatus->userid,tstatus->tid,_myid);
}
void Chatter::CancelTask(tUInt32 tstatusid)
{
	if (!_stconn)
		return ;

	tTStatus* tstatus;

	if (!(tstatus=_tstatusmgr->QueryTStatus(tstatusid)))
		return ;
	if (tstatus->isin())
		_stconn->TerminateTask(tstatus->userid,tstatus->tid,_myid);
	else 
		_stconn->CancelTask(tstatus->userid,tstatus->tid,_myid);
}
tBool Chatter::AmIAdmin(void) const
{
	return _groupmgr->AmIAdmin();
}
void Chatter::SetChatBusy(tBool val)
{
	_is_building_session = val;
}
void Chatter::SetMeAdmin(void)
{
	_groupmgr->SetMeAdmin();
}
void Chatter::ResetMeAdmin(void)
{
	_groupmgr->ResetMeAdmin();
}
tBool Chatter::SetUserAdmin(tUInt32 userid)
{
	return _groupmgr->SetUserAdmin(userid);
}
tBool Chatter::CanIInviteVoice(string& reasoninfo)
{
	if (_is_building_session)
	{
		reasoninfo = "同时只能创建一个会话.";
		return FALSE;
	}

	if (!_groupmgr->IsEmpty())
	{
		if (_groupmgr->AmIAdmin())
		{
			reasoninfo = "您不是管理员,不能邀请新成员加入.";
			return FALSE;
		}
		else if (!_groupmgr->GetGroupType()!=GroupMgr::GROUP_TYPE_VOICE)
		{
			reasoninfo = "您有一个非语音会话正在进行中,不能发起语音会话.";
			return FALSE;
		}
	}

	return TRUE;
}
tBool Chatter::CanIInviteVideo(string& reasonifno)
{
	return TRUE;
}
void Chatter::NotifyAddNewMember(tUInt32 userid,string username,string host,tUInt32 port)
{}
void Chatter::AddMember(tUInt32 userid,string username,string public_ip,tUInt32 public_pt_udp_port,string internal_ip,tUInt32 internal_pt_udp_port)
{
	_groupmgr->AddMember2Add(userid,username,public_ip,public_pt_udp_port,internal_ip,internal_pt_udp_port);
}
void Chatter::DeleteMember(tUInt32 userid)
{
	_groupmgr->DeleteMemberById(userid);

	if (_groupmgr->IsEmpty())
		CloseVoice();
}
tBool Chatter::AmIInChatting(void) const
{
	return (!_groupmgr->IsEmpty());
}
void Chatter::Execute(Thread::Arg arg)
{
	/////////////////////////////////////Local variables////////////////////////////
	SecUdpNeter*	secudpneter=NULL;
	tUInt32 tid=0;
	string log_file_path_and_name;

	tTaskType tasktype;
	tTState tstate;
	float	progress=0;
	tBridgeMsg* reqestmsg = NULL;
	tBool exit_flag = FALSE;

	////////////////////////////////////////////////////////////////////////////////
	// read configurations
	////////////////////////////////////////////////////////////////////////////////
	if (!ReadConfigFile())
    {
      exit(1);
    }

	//////////////////////////////logfile///////////////////////////////////////////
	log_file_path_and_name = _myname+"_chatter.log";
	::DeleteFileA(log_file_path_and_name.c_str());
	_filelog = new FileLog(log_file_path_and_name);
	_filelog->SetMode(debug);
	////////////////////////////////////////////////////////////////////////////////

	LOGINFO("Chatter starting up...");


	LOGDEBUG("Chatter::Execute:Configurations:");
	LOGDEBUG("CentralSHost:"<<centrals_host);
	LOGDEBUG("CentralSCPort:"<<centrals_cport);

	SetChatterState(CHATTER_STATE_STARTINGUP);

	_groupmgr = new GroupMgr(_filelog);
	_tstatusmgr = new TStatusMgr(_update_func);

	/**
	 * At the very beginning, the axchatter has to connect to the Central Server
	 * for punching a hole for peer members to Tcp connect or send Udp voice data
	 */
	LOGINFO("Chatter logining...");

	_stconn = new STConn(centrals_host,centrals_cport,this,_tstatusmgr,_filelog);

	_stconn->Start();

	tid = _stconn->Login(_myid,_myname);

	if (tid==0)
	{
		LOGERROR("Chatter Failed to login, aborting...");
		Shutdown();
	}
	else
	{
		tTStatus* tstatus=NULL;
		while (!exit_flag)
		{
			if (!(tstatus=_tstatusmgr->QueryOutTStatus(CENTRALS_USER_ID,tid)))
				LOGERROR("Chatter Failed to QueryUserTStatusList for login.");

			switch(tstatus->tstate)
			{
			case TASK_STATE_REQUEST_CONFIRMED:
			case TASK_STATE_FINISHED:
				LOGINFO("Chatter logined successfully.");
				exit_flag = TRUE;
				break;
			case TASK_STATE_REQUEST_CREATED:
			case TASK_STATE_REQUEST_SENT:
				break;
			case TASK_STATE_REQUEST_REJECTED:
			case TASK_STATE_REQUEST_TIMEOUTED:
			case TASK_STATE_CANCELLED:
			case TASK_STATE_TERMINATED:
				LOGERROR("Chatter login failed,aborting...");
			default:
				LOGERROR("Chatter login got unknown state "<<tstatus->tstate<<".");
				exit_flag = TRUE;
				Shutdown();
				break;
			}

			Thread::Yield();
		}
	}

	LOGINFO("Chatter::Execute:Started...");

	while (!_is_shutdown_requested)
	{
		SetChatterState(CHATTER_STATE_RUNNING);
		// Nothing to to, just looping ...
		Sleep(1);
	}

	// Here a shutdown singnal received.
	LOGINFO("Chatter::Execute:shutdown singnal received,shutting down...");

	_chatter_state = CHATTER_STATE_SHUTTINGDOWN;

	if (_mvhandler)		_mvhandler->Shutdown();
	if (_recorder)	_recorder->Shutdown();
	if (_recordhandler)	_recordhandler->Shutdown();

	tUInt32 livecounter=4;
	tUInt32 counter = 0;
	while (counter != 0 && counter++ < 10)
	{
		if(_mvhandler && Thread::FINISHED_STATE == _mvhandler->State())
		{
			livecounter--;
			_mvhandler->Join();
			delete _mvhandler;
			_mvhandler = NULL;
		}
		if(_recorder && Thread::FINISHED_STATE == _recorder->State()) 
		{
			livecounter--;
			_recorder->Join();
			_recorder->UnInit();
			delete _recorder;
			_recorder = NULL;
		}
		if(_recordhandler && Thread::FINISHED_STATE == _recordhandler->State())
		{
			livecounter--;
			_recordhandler->Join();
			_recordhandler->UnInit();
			delete _recordhandler;
			_recordhandler = NULL;
		}
	}

	if (livecounter)
		LOGWARNING("Chatter::Execute:Some thread(s) won't end,but sorry we will not join any longer...");

	LOGINFO("Chatter shutdowned.");

	if (_recorder)		delete _recorder;
	if (_mvhandler)		delete _mvhandler;
	if (_recordhandler)	delete _recordhandler;
	if (_vudpsocket)	delete _vudpsocket;
	if (_groupmgr)		delete _groupmgr;
	if (_filelog)		delete _filelog;

	SetChatterState(CHATTER_STATE_FINISHED);
}
void Chatter::SetChatterState(tChatterState chatterstate)
{
	if (_update_func&&chatterstate!=_chatter_state)
	{
		_update_func(0);
	}

	_chatter_state = chatterstate;
}