#pragma once
#include "IoHandler.h"
#include "NetAddress.h"

class Session : public IIoHandler
{
public:
	Session();
	virtual ~Session();

	virtual void OnIoCompleted(class OverlappedBase* overlapped, uint32 numOfBytes) override;
	virtual HANDLE GetHandle() const override;

	SOCKET GetSocket() const { return _socket; }

	void				SetNetAddress(const NetAddress& addr) { _netAddress = addr; }
	const NetAddress&	GetNetAddress() const { return _netAddress; }
	NetAddress&			GetNetAddress() { return _netAddress; }

public:
	char _recvBuffer[4096]; // TEMP

private:
	SOCKET			_socket = INVALID_SOCKET;
	NetAddress		_netAddress = {};
	Atomic<bool>	_isConnected = false;
};

