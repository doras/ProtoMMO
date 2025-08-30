#include "pch.h"
#include "NetAddress.h"

#include <WS2tcpip.h>

NetAddress::NetAddress(const String& ip, uint16 port)
{
	::memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = IP2Addr(ip.c_str());
	_sockAddr.sin_port = ::htons(port);
}

String NetAddress::GetIP() const
{
	wchar_t ip[INET_ADDRSTRLEN] = {};
	::InetNtopW(AF_INET, &_sockAddr.sin_addr, ip, INET_ADDRSTRLEN);
	return String(ip);
}

IN_ADDR NetAddress::IP2Addr(const wchar_t* ip)
{
	IN_ADDR addr = {};
	::InetPtonW(AF_INET, ip, &addr);
	return addr;
}
