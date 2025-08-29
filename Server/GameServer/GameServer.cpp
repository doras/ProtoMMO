#include "pch.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <iostream>

int main()
{
	// Initialize Winsock
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	// Create a listen socket
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 err = ::WSAGetLastError();
		std::cout << "socket failed, err=" << err << std::endl;

		::WSACleanup();
		return 1;
	}

	// Bind the socket
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 err = ::WSAGetLastError();
		std::cout << "bind failed, err=" << err << std::endl;
		::closesocket(listenSocket);
		::WSACleanup();
		return 1;
	}

	// Start listening
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		int32 err = ::WSAGetLastError();
		std::cout << "listen failed, err=" << err << std::endl;
		::closesocket(listenSocket);
		::WSACleanup();
		return 1;
	}

	// --------- Server is now set up and listening for connections ---------

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 clientAddrLen = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int32 err = ::WSAGetLastError();
			std::cout << "accept failed, err=" << err << std::endl;
			continue; // Continue accepting other connections
		}

		char clientIp[INET_ADDRSTRLEN];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, sizeof(clientIp));
		std::cout << "Client connected: " << clientIp << ":" << ::ntohs(clientAddr.sin_port) << std::endl;
	}

	// -------- Clean up and exit ---------

	// Clean up Winsock
	::WSACleanup();

	return 0;
}
