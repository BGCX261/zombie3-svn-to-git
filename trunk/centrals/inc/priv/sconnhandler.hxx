#ifndef __SLAVE_CONN_HANDLER__
#define __SLAVE_CONN_HANDLER__

#include "mytypes.h"
#include "priv/connhandler.hxx"

class SConnManager;
class TcpSocket;

class SConnHandler	:	public ConnHandler
{
public:
	//

	/**
	 * @brief	:	Constructor
	 * @param manager	:
	 * @param socket	:
	 */
	SConnHandler(SConnManager* manager,TcpSocket* socket,FileLog* filelog);

	/**
	 * @brief	:	Destructor
	 */
	virtual ~SConnHandler();

	tUInt32 GetSlaveId(void) const;
	string	GetSlaveHost(void) const;
	tUInt32	GetSlavePort(void) const;
	tUInt32 GetSlaveLoad(void) const;

protected:
	//
	/********************************************************************************************/
	virtual void PreRunning(void);
	virtual void AfterRunning(void);

	virtual tBool ProcessIMsg(void);
	virtual tBool DoOTask(void);
	/********************************************************************************************/

private:
	//
	SConnManager*	_manager;
	TcpSocket*		_socket;

	tUInt32 _slave_id;
	string	_slave_host;
	tUInt32	_slave_port;

	tUInt32 _load;
};
#endif //__SLAVE_CONN_HANDLER__