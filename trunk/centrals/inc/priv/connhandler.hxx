#ifndef __TCONN_HXX__
#define __TCONN_HXX__

#include "mytypes.h"
#include "mythread.hxx"
#include "was_api.h"
#include "safeque.hxx"
#include "bridgenet.hxx"
//#include "taskdefs.h"

class TcpBridgeNet;
class FileLog;

class ConnHandler	:	public Thread
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
	ConnHandler(FileLog* filelog);

	/**
	 * \brief	Destructor
	 */
	virtual ~ConnHandler();

	void	Shutdown(void);

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

	virtual void PreRunning();
	virtual void AfterRunning();

	virtual tBool ProcessIMsg(void);
	virtual tBool DoOTask(void);
	/********************************************************************************************/
	
	//void	GetMemInfoFromMsg(tBridgeMsg* msg,tMemInfo** meminfo);

	void	SaveOMsg(tBridgeMsg* bmsg);
	void	SaveIMsg(tBridgeMsg* bmsg);
	
	void	ClearTConn(void);

	tUInt32 GetASocketId(void);

	FileLog*			_filelog;
	
	tBridgeMsgQue		_imsg_que;
	tBridgeMsgQue		_omsg_que;
	tBridgeMsg*			_imsg;
	tBridgeMsg*			_omsg;

private:
	//
	void ClearOMsgQue(void);
	void ClearIMsgQue(void);
	

	tBool _is_shutdown_requested;
};
#endif //__TCONN_HXX__