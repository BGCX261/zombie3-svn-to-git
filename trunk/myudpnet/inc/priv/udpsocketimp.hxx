#ifndef __UDP_SOCKET_IMP_HXX__
#define __UDP_SOCKET_IMP_HXX__

//#include "mydll.h"
#include "mytypes.h"
#include "udpsocketexception.hxx"

/**
 * to invalidate the winsock.h in windows.h file
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <string>
#include <sstream>

typedef unsigned long tInetAddr;

class UdpSocketImp
{
public:
	/// \brief Constructor
	UdpSocketImp();

	/// \brief Constructor for server
	/// Create the SOCKET, then bind it to the given address and port
	/// port	: the port to bind the socket to
	// UdpSocketImp(tInt32 port,FileLog* filelog = NULL);

	/// \brief Destructor
	~UdpSocketImp();

	/// \brief Send data to a specific destinate host
	/// data	: the data to send
	/// size	: the length of the data to send
	/// address_remote	:	the destinate address to send to
	/// port_remote		:	the port of the destinate host
	virtual tInt32 SendTo(tByte* data, tInt32 size,string address_remote, tInt32 port_remote);

	/// \brief Send data to a specific destinate host
	/// data	: the data to send
	/// size	: the length of the data to send
	/// address_remote	:	the destinate address to send to
	virtual tInt32 SendTo(tByte* data, tInt32 size,const sockaddr_in* address_remote);

	/// \brief Send data to the previously shot destinate endpoint.
	/// data	: the data to send
	/// size	: the length of the data to send
	//virtual tInt32 SendTo(tByte* data, tInt32 size);

	/// \brief Receive data on current listening port
	/// data	: the data received
	/// size	: the length of the received data
	/// return	: zero indicates success, or non-zero.
	virtual tInt32 RecvFrom(tByte* buf, tInt32 bufsize, string &host_remote, tInt32 &port_remote);

	virtual int IsReadyForRead(unsigned int ms);

	virtual int IsReadyForWrite();

	bool UdpSocketImp::SetReuseAddr();

	bool UdpSocketImp::ClearReuseAddr();

	SOCKET UdpSocketImp::GetHandle();

	virtual void Close();

	tInt32 Bind(tInt32 port);

    unsigned short PeerLocalPort() const;

	string PeerLocalHost() const;

private:
	/// \brief Instantiate a windows socket with UDP parameters
	tInt32 Socket();
	tInt32 SetBroadcast();

	SOCKET _socket;

	// for avoiding constructing a sockaddr_in
	// for sendto() each time
	sockaddr_in _sockaddr_remote;
	string _host_remote;
	string _port_remote;
};

#endif //__UDP_SOCKET_IMP_HXX__