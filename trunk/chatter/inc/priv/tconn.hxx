#ifndef __TCONN_HXX__
#define __TCONN_HXX__

//#include "was_api.h"
#include "mytypes.h"
#include "mythread.hxx"
#include "safeque.hxx"
//#include "taskdefs.h"
#include "bridgenet.hxx"
#include "bridgenet.hxx"

class FileLog;

class TConn	:	public Thread
{
public:
	//
	//////////////////////////////////TYPEDEFS////////////////////////////////////////////////////
	typedef SafeQue<tBridgeMsg*> tBridgeMsgQue;
	//////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *\brief	Constructor
	 * @param remote_host	:	the address to connect to
	 * @param port			:	the remote listening tcp port
	 */
	TConn(string remote_host,tUInt32 remote_port,FileLog* filelog);

	/**
	 * \brief	Destructor
	 */
	virtual ~TConn();

	void	Shutdown(void);

	string	GetRemoteHost(void) const;
	tUInt32	GetRemotePort(void) const;

	virtual void Disconnect(void);

	static	tUInt32 GetATId(void);

protected:
	//
	/********************************************************************************************
	 *****************************Construct a FrameWork******************************************
	 ********************************************************************************************/
	void Execute(Thread::Arg arg);
	tBool HandleIncomming(void);
	tBool HandleOutgoing(void);

	virtual tBool RecvBridgeMsg(void);
	virtual tBool SendBridgeMsg(void);

	virtual void PreRunning(void);
	virtual void AfterRunning(void);

	virtual tBool ProcessIMsg(void);
	virtual tBool DoOTask(void);
	/********************************************************************************************/
	
	//static void GetMemInfoFromMsg(tBridgeMsg* msg,tMemInfo** meminfo);

	void	SaveOMsg(tBridgeMsg* bmsg);
	void	SaveIMsg(tBridgeMsg* bmsg);
	
	void	ClearTConn(void);

	//
	string				_remote_host;
	tUInt32				_remote_port;
	FileLog*			_filelog;
	
	tBridgeMsgQue		_imsg_que;
	tBridgeMsgQue		_omsg_que;
	tBridgeMsg*			_imsg;
	tBridgeMsg*			_omsg;

private:
	//
	void ClearOMsgQue(void);
	void ClearIMsgQue(void);
	
	/**
	 * the out going task id incrementor
	 */
	static tUInt32	_tid_counter;

	tBool _is_shutdown_requested;
};
#endif //__TCONN_HXX__