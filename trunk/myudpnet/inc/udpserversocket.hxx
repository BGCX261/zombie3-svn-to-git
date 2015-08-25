#ifndef __UDP_SERVER_SOCKET_HXX__
#define __UDP_SERVER_SOCKET_HXX__

#include "udpsocket.hxx"

class EXPORT_MYUDPNET UdpServerSocket : public UdpSocket
{
public:

	/// \brief Constructor
	UdpServerSocket(tInt32 port);

	/// \brief Destructor
	virtual ~UdpServerSocket();

	/// \brief Send data to a specific address
	///virtual int SendTo(string address_remote, int port_remote);

	/// \brief Receive data on current listening port
	///virtual int RecvFrom();
protected:
	tInt32 Bind(tInt32 port);

private:
	// Hide default constructor and copy constructor
	UdpServerSocket();

	UdpServerSocket(const UdpServerSocket &model);	
};


#endif //__UDP_SERVER_SOCKET_HXX__