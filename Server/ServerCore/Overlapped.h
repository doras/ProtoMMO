#pragma once

#include "IoHandler.h"

class OverlappedBase : public OVERLAPPED
{
public:
	enum class IOType : uint8
	{
		Connect,
		Disconnect,
		Accept,

		Recv,
		Send,
	};

public:
	OverlappedBase(IOType type);

	void Init();

public:
	IOType	type;
	IIoHandlerPtr owner; // The owner of this overlapped operation
};

class ConnectOverlapped : public OverlappedBase
{
public:
	ConnectOverlapped() : OverlappedBase(IOType::Connect) {}
};

class DisconnectOverlapped : public OverlappedBase
{
public:
	DisconnectOverlapped() : OverlappedBase(IOType::Disconnect) {}
};

class AcceptOverlapped : public OverlappedBase
{
public:
	AcceptOverlapped() : OverlappedBase(IOType::Accept) {}

public:
	SessionPtr session; // The session to accept the connection into
};

class RecvOverlapped : public OverlappedBase
{
public:
	RecvOverlapped() : OverlappedBase(IOType::Recv) {}
};

class SendOverlapped : public OverlappedBase
{
public:
	SendOverlapped() : OverlappedBase(IOType::Send) {}

public:
	Vector<BYTE> buffer; // TEMP
};
