#include "config.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"

#include "udpsocket.hxx"
#include "wfaudiorecorder.hxx"
#include "wfaudioplayer.hxx"

#include "priv/chatmember.hxx"
#include "priv/groupmgr.hxx"
#include "priv/recordhandler.hxx"


RecordHandler::RecordHandler(AudioRecorder* _recorder,UdpSocket*udpsocket,GroupMgr* groupmgr,FileLog* filelog)
:	Thread(),
	_audiorecorder(_recorder),
	_udpsocket(udpsocket),
	_groupmgr(groupmgr),
	_converter(NULL),
	_is_shutdown_requested(FALSE),
	_filelog(filelog)
{
}
RecordHandler::RecordHandler(const RecordHandler &model)
{
}
RecordHandler::~RecordHandler()
{
}
tInt32 RecordHandler::Init(void)
{
	_converter = new SpeexConverter(QUALITY,_filelog);

	if (_converter->Init() < 0)
	{
		LOGERROR("RecordHandler::Init:Failed to init the converter.");
		return -1;
	}

	return 1;
}
void RecordHandler::UnInit(void)
{
	if (_converter)
	{
		_converter->UnInit();
		delete _converter;
		_converter = NULL;
	}
}
void RecordHandler::Execute(Thread::Arg arg)
{
	WAVEHDR* pwh;
	tByte* echo;
	tUInt32 count = 0;
	tUInt32 sentsize;
	tUInt32 size = 0;

	tByte buffer[2048]={0};
	tUInt32 buffer_len = 2048;
	tByte buffer2[2048]={0};
	tUInt32 buffer_len2 = 2048;

	ChatMember* member = NULL;
	GroupMgr::tHostChatMemberMap hostchatmembermap;
	GroupMgr::tHostChatMemberMapIter hostchatmemberiter;

	LOGINFO("RecordHandler starting up...");

	if (Init()<0)
	{
		LOGERROR("RecordHandler::Execute:Failed to init,shutting down...");
		Shutdown();
	}

	MSG msg;
	while(!_is_shutdown_requested && GetMessage(&msg,0,0,0))
	{
		switch(msg.message )
		{
		case WIM_OPEN:
			LOGDEBUG("RecordHandler::Execute:	WOM_OPEN");
			break;
		case WIM_CLOSE:
			LOGDEBUG("RecordHandler::Execute:	WOM_CLOSE");
			break;
		case WIM_DATA:
			pwh=(WAVEHDR*)msg.lParam;
			LOGDEBUG("RecordHandler::Execute:	WIM_DATA  size="<<pwh->dwBytesRecorded);


			/**
			 * 1 convert (compress) the voice data
			 * 2 decrement the free block count
			 * 3 send the voice data to all other group members
			 */
			LOGDEBUG("RecordHandler::Execute:Memorize current playing pos for all palyers");

			_groupmgr->SetAllPlayerCurPos();


			LOGDEBUG("RecordHandler::Execute:Cancel the echos.");

			hostchatmembermap = _groupmgr->GetHostChatMemberMap();

			for (hostchatmemberiter=hostchatmembermap.begin(); hostchatmemberiter!=hostchatmembermap.end();hostchatmemberiter++)
			{
				member = hostchatmemberiter->second;

				echo = member->GetPlayer()->GetLatestData((tUInt32)pwh->dwBytesRecorded);

				if (echo)
					_converter->CancelEcho((tByte*)pwh->lpData,echo,(tUInt32)pwh->dwBytesRecorded);
			}

			LOGDEBUG("RecordHandler::Execute:Encode the voice data.");

			size = _converter->Code((tByte*)pwh->lpData,(tUInt32)pwh->dwBytesRecorded,buffer,buffer_len);

			for (hostchatmemberiter=hostchatmembermap.begin(); hostchatmemberiter!=hostchatmembermap.end();hostchatmemberiter++)
			{
				member = hostchatmemberiter->second;

				sentsize = _udpsocket->SendTo(buffer,size,member->GetSockaddr());;

				if (sentsize != size)
					LOGWARNING("RecordHandler::Execute:Sent "<<sentsize<<" out of "<<size<<" bytes to '"<<member->GetMemHost()<<"::"<<member->GetMemPort()<<"'.");
				else
					LOGDEBUG("RecordHandler::Execute:"<<size<<" bytes have been sent to member '"<<member->GetMemHost()<<"::"<<member->GetMemPort()<<"'.");
			}

			_audiorecorder->IncrFreeBlockCount();

			break;
		case USER_SHUTDOWN:
			LOGDEBUG("RecordHandler::Execute:	USER_SHUTDOWN \n");
			/**
			 * Nothing to do, just an event to release blocked GetMessage
			 * shutdown_requested flag has already been set by shutdown()
			 */
			break;
		}
	}

	UnInit();

	LOGINFO("RecordHandler shutdowned.");
}
void RecordHandler::Shutdown()
{
	_is_shutdown_requested = TRUE;

	/**
	 * As the flag set, send an nofification event to the running thread
	 */
	PostThreadMessage((tUInt32)RealThreadId(),USER_SHUTDOWN,0,0);
}
