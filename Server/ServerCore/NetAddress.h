#pragma once

#include <WinSock2.h>
#include "Container.h"

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr) {}
	NetAddress(const String& ip, uint16 port);

	const SOCKADDR_IN&	GetSockAddr() const { return _sockAddr; }
	SOCKADDR_IN&		GetSockAddr() { return _sockAddr; }
	String				GetIP() const;
	uint16				GetPort() const { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR IP2Addr(const wchar_t* ip);

private:
	SOCKADDR_IN _sockAddr = {};
};

