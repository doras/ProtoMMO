#include "pch.h"
#include "SocketUtils.h"
#include "NetAddress.h"

#pragma comment(lib, "ws2_32.lib")

LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);

	// Load extension functions
	SOCKET tempSocket = CreateSocket();
	ASSERT_CRASH(tempSocket != INVALID_SOCKET);
	ASSERT_CRASH(LoadExtensionFunction(tempSocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(LoadExtensionFunction(tempSocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(LoadExtensionFunction(tempSocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	CloseSocket(tempSocket);
}

void SocketUtils::Cleanup()
{
	::WSACleanup();
}

bool SocketUtils::LoadExtensionFunction(SOCKET socket, GUID guid, LPVOID* ppfn)
{
	DWORD bytes = 0;
	int32 result = ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&guid, sizeof(guid),
			ppfn, sizeof(ppfn), 
			&bytes, NULL, NULL);
	return (result != SOCKET_ERROR);
}

SOCKET SocketUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

void SocketUtils::CloseSocket(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
	{
		::closesocket(socket);
		socket = INVALID_SOCKET;
	}
}

bool SocketUtils::Bind(SOCKET socket, NetAddress netAddr)
{
	if (socket == INVALID_SOCKET)
	{
		return false;
	}

	return (::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN)) 
		!= SOCKET_ERROR);
}

bool SocketUtils::BindAnyAddr(SOCKET socket, uint16 port)
{
	if (socket == INVALID_SOCKET)
	{
		return false;
	}

	SOCKADDR_IN addr;
	::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	addr.sin_port = ::htons(port);

	return (::bind(socket, reinterpret_cast<const SOCKADDR*>(&addr), sizeof(addr))
		!= SOCKET_ERROR);
}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	if (socket == INVALID_SOCKET)
	{
		return false;
	}
	return (::listen(socket, backlog) != SOCKET_ERROR);
}

bool SocketUtils::SetLinger(SOCKET socket, bool flag, uint16 seconds)
{
	if (socket == INVALID_SOCKET)
	{
		return false;
	}

	LINGER l = {};
	l.l_onoff = flag ? 1 : 0;
	l.l_linger = seconds;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, l);
}

bool SocketUtils::SetReuseAddr(SOCKET socket, bool flag)
{
	if (socket == INVALID_SOCKET)
	{
		return false;
	}
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	if (socket == INVALID_SOCKET)
	{
		return false;
	}
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	if (socket == INVALID_SOCKET)
	{
		return false;
	}
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	if (socket == INVALID_SOCKET)
	{
		return false;
	}
	return SetSockOpt(socket, IPPROTO_TCP, TCP_NODELAY, flag);
}

bool SocketUtils::SetUpdateAcceptContext(SOCKET socket, SOCKET listenSocket)
{
	if (socket == INVALID_SOCKET || listenSocket == INVALID_SOCKET)
	{
		return false;
	}
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}
