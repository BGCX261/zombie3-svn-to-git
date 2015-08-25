#ifndef _UDP_SOCKET_HXX_
#define _UDP_SOCKET_HXX_

//#include "mydll.h"
#if defined(WIN32)

#if  defined(MYUDPNET_EXPORTS)
/* DLL export */
#ifdef EXPORT_MYUDPNET
#undef EXPORT_MYUDPNET
#endif
#define EXPORT_MYUDPNET __declspec(dllexport)
#else
/* EXE import */
#define EXPORT_MYUDPNET __declspec(dllimport)
//#define EXPORT
#endif

#ifdef WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif

#else
#define EXPORT_MYUDPNET 
#endif

#include "mytypes.h"
#include <string>
#include <assert.h>

using namespace std;

#if defined(linux)
typedef int SOCKET;
#elif defined(WIN32)
/**
 * to invalidate the winsock.h in windows.h file
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#else 
#error "Unsupported OS"
#endif

class EXPORT_MYUDPNET UdpSocket
{
public:

	/// \brief Constructor
	UdpSocket();

	/// \brief Constructor
	//UdpSocket(tInt32 port,FileLog* filelog = NULL);

	/// \brief Destructor
	~UdpSocket();

	//virtual int Bind(int port);

	virtual tInt32 SendTo(tByte* data, tInt32 size,string address_remote, tInt32 port_remote);

	virtual tInt32 SendTo(tByte* data, tInt32 size,const sockaddr_in* sockaddr_remote);

	virtual tInt32 RecvFrom(tByte* buf, tInt32 bufsize, string &host_remote, tInt32 &port);

	virtual int IsReadyForRead(unsigned int ms = 0);

	virtual int IsReadyForWrite();

	virtual bool SetReuseAddr();

	virtual bool ClearReuseAddr();

	virtual SOCKET GetHandle();

	// when communication finished, close the socket.
	virtual void Close();

    virtual unsigned short PeerLocalPort() const;

	virtual string PeerLocalHost() const;

protected:
	friend class UdpSocketImp;
	UdpSocketImp* _udpsocketimp;

private:
	/// to force to use new instead of static constructor
	UdpSocket(const UdpSocket &model);
};

#endif //_UDP_SOCKET_HXX_