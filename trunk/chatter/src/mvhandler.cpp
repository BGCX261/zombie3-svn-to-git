#include "config.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "wfaudioplayer.hxx"
#include "udpsocket.hxx"
#include "udpbridgenet.hxx"
#include "secudpneter.hxx"
#include "priv/speexconverter.hxx"
#include "priv/chatmember.hxx"
#include "priv/groupmgr.hxx"
#include "priv/tconn.hxx"
#include "priv/mvhandler.hxx"
#include "chatter.hxx"

MVHandler::MVHandler(Chatter* chatter,tUInt32 myid,string centrals_host,tUInt32 centrals_udp_port,UdpSocket* udpsocket,GroupMgr* groupmgr,FileLog* filelog)
		:	Thread(),
			_chatter(chatter),
			_myid(myid),
			_centrals_host(centrals_host),
			_centrals_cport(centrals_udp_port),
			_udpsocket(udpsocket),
			_groupmgr(groupmgr),
			_is_shutdown_requested(FALSE),
			_filelog(filelog)
{}

MVHandler::~MVHandler()
{
}
void MVHandler::Shutdown(void)
{
	_is_shutdown_requested = TRUE;;
}

void MVHandler::Execute(Thread::Arg arg)
{
	SpeexConverter* converter;

	tUInt32 userid;
	string username;

	string remote_host;
	tInt32 remote_port;

	tByte buf[4000];
	tUInt32 bufsize = 4000;

	tInt32 datasize;

	tByte decodedbuf[4000];
	tInt32 decodedsize;

	LOGINFO("MVHandler starting up...");

	converter = new SpeexConverter(QUALITY,_filelog);

	if (converter->Init() < 0)
	{
		LOGERROR("MVHandler::Execute:Failed to init the converter,shutting down...");
		Shutdown();
	}

	GroupMgr::tDoubleEndpoints const* doubleends;
	tUInt32 counter=250;
	tUInt32 renewcounter = 0;
	while (!_is_shutdown_requested)
	{
		/**Sending punching data to all 2Added members,do it every 250 loops.*/
		if (++counter>250)
		{
			counter=0;
			for (tInt32 i=_groupmgr->Member2AddNum()-1;i>=0;i--)
			{
				doubleends = _groupmgr->GetDoubleEndpointsAt(i);

				_udpsocket->SendTo((tByte*)"punchinghole",12,doubleends->internal_ip,doubleends->internal_udp_port);
				_udpsocket->SendTo((tByte*)"punchinghole",12,doubleends->public_ip,doubleends->public_udp_port);

				LOGDEBUG("MVHandler::Execute:Test data has been sent to '"<<doubleends->public_ip<<"::"<<doubleends->public_udp_port<<" and "
					<<doubleends->internal_ip<<"::"<<doubleends->internal_udp_port<<"'.");
			}
		}

		/**since it's time consuming,do it every 1000*10 times.*/
		if (++renewcounter==10000)
		{
			renewcounter = 0;
			RenewEndpointsMapping();
		}

		if (!_udpsocket->IsReadyForRead())
		{
			Yield();
			continue;
		}

		
		// Here we know there are some data arrived,just try to get it
		datasize = _udpsocket->RecvFrom(buf,bufsize,remote_host,remote_port);
		int err = GetLastError();

		if (datasize < 0)
			LOGERROR("MVHandler::Execute:_udpnet->RecvFrom error happened, ret="<<datasize);
		else if (datasize == 0)
			LOGDEBUG("MVHandler::Execute:_udpnet->RecvFrom got a shutdown gracefully by peer");
		else
		{
			LOGDEBUG("MVHandler::Execute:received "<<datasize<<" bytes from "<<remote_host<<"::"<<remote_port);

			if (_groupmgr->AnyMember2Add() && _groupmgr->Try2AddMember2Add(remote_host,remote_port,userid,username))
			{
				_chatter->NotifyAddNewMember(userid,username,remote_host,remote_port);
			}
				

			/**
			 * Process it.
			 * 1) find the right chatmember.
			 * 2) decode the received voice data.
			 * 3) write the voice data into the right palyer.
			 */
			decodedsize = converter->Decode(buf,datasize,decodedbuf,bufsize);

			if (decodedsize <= 0)
				LOGWARNING("MVHandler::Execute:Failed to decode received data,abandon it.");
			else
			{
				LOGDEBUG("MVHandler::Execute:After decode,we got "<<decodedsize<<" bytes voice data.");

				if (!_groupmgr->SetVoiceDataByHost(remote_host,decodedbuf,decodedsize))
					LOGWARNING("MVHandler::Execute:Failed to set the receiced voice data into a proper player,abandon it.");
			}
		}
	}

	LOGDEBUG("MVHandler::Execute:Shutdown signal received.");

	if (converter)
	{
		converter->UnInit();
		delete converter;
		converter = NULL;
	}
	
	LOGINFO("MVHandler shutdowned.");
}
tBool MVHandler::RegisterUdpEndpoints()
{
	tUInt32 len;
	tByte* msg;
	tUInt32 internal_port;
	SecUdpNeter* udpneter;

	LOGDEBUG("MVHandler::RegisterUdpEndpoints:Started...");

	/** send test data to make sure the udp socket is locally bound to a port.*/
	RenewEndpointsMapping();

	internal_port	= _udpsocket->PeerLocalPort();
	udpneter = new SecUdpNeter(_udpsocket,10000,_filelog);
	udpneter->Start();

	/**
	 * Prepare the register msg,just to attach internal udp port,for internal udp host is the 
	 * same with internal tcp host address.
	 */
	tUInt32 tid=TConn::GetATId();

	tChar myid_str[10] = {0};
	sprintf_s(myid_str, "%d", _myid);
	tChar internal_port_str[10] = {0};
	sprintf_s(internal_port_str, "%d", internal_port);

	msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_ENDPOINTS_REGISTER_REQ,tid, X_WAS_RSPC_NONE,
								E_WAS_TAG_ID_MYID, strlen(myid_str), myid_str, 
								E_WAS_TAG_ID_IPOT, strlen(internal_port_str), internal_port_str,
								E_WAS_TAG_ID_NONE);

	tBridgeMsg* bmsg = CreateBridgeMsg();

	bmsg->lom = len;
	bmsg->mpi = E_WAS_MSG_FIRST_PART;
	bmsg->tid = tid;
	bmsg->rspc= X_WAS_RSPC_NONE;
	bmsg->msg = msg;
	bmsg->tom = E_WAS_MSG_ENDPOINTS_REGISTER_REQ;
	bmsg->isReceived = FALSE;

	udpneter->SendTo(bmsg,_centrals_host,_centrals_cport);

	/**
	 * Send a register msg to UdpListener,and wait for answer
	 */
	tBridgeMsg* imsg;
	tUInt32 counter = 0;
	tBool succeeded = FALSE;
	while (!succeeded && counter<100)
	{
		counter++;

		SleepMs(100);	//wait for a while
		
		if (!udpneter->Recv(&imsg))
			continue;

		switch (imsg->tom)
		{
		case E_WAS_MSG_ENDPOINTS_REGISTER_CNF:
			if (imsg->rspc==X_WAS_RSPC_OK)
			{
				LOGDEBUG("MVHandler::RegisterUdpEndpoints:Register succeeded.");
				succeeded = TRUE;
			}
			else
			{
				LOGWARNING("MVHandler::RegisterUdpEndpoints:Failed to register Udp Endpoints,rspc from Centrals is '"<<imsg->rspc<<"'.");
				succeeded = FALSE;
			}
			break;
		default:
			LOGWARNING("MVHandler::RegisterUdpEndpoints:Received unknown msg from with type "<<imsg->tid<<".");
			break;			
		}

		ReleaseBridgeMsg(&imsg);
	}

	udpneter->Shutdown();

	counter=0;
	while (counter<100)
	{
		++counter;

		if (udpneter->State()!=Thread::FINISHED_STATE)
			Yield();
		else
		{
			udpneter->Join();
			delete udpneter;
			udpneter=NULL;
			break;
		}
	}

	if (counter>=100)
		LOGWARNING("MVHandler::RegisterUdpEndpoints:Udpneter won't end,sorry we will not join any longer,force shutting down...");

	LOGDEBUG("MVHandler::RegisterUdpEndpoints:Ended.");

	return succeeded;
}


void MVHandler::RenewEndpointsMapping(void)
{
	if (_udpsocket)
		_udpsocket->SendTo((tByte*)"test",4,_centrals_host,9999);
	else 
		LOGWARNING("MVHandler::RenewEndpointsMapping:no udpsocket created.");
}