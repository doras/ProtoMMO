#pragma once

#include <WinSock2.h>
#include <MSWSock.h>

class SocketUtils
{
public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;

public:
	static void	Init();
	static void	Cleanup();

	static bool		LoadExtensionFunction(SOCKET socket, GUID guid, LPVOID* ppfn);
	static SOCKET	CreateSocket();
	static void		CloseSocket(SOCKET& socket);

	static bool		Bind(SOCKET socket, class NetAddress netAddr);
	static bool		BindAnyAddr(SOCKET socket, uint16 port);
	static bool		Listen(SOCKET socket, int32 backlog = SOMAXCONN);

	static bool		SetLinger(SOCKET socket, bool flag, uint16 seconds);
	static bool		SetReuseAddr(SOCKET socket, bool flag);
	static bool		SetRecvBufferSize(SOCKET socket, int32 size);
	static bool		SetSendBufferSize(SOCKET socket, int32 size);
	static bool		SetTcpNoDelay(SOCKET socket, bool flag);
	static bool		SetUpdateAcceptContext(SOCKET socket, SOCKET listenSocket);
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optname, const T& value)
{
	return (::setsockopt(socket, level, optname, 
			reinterpret_cast<const char*>(&value), sizeof(T)) 
		!= SOCKET_ERROR);
}
