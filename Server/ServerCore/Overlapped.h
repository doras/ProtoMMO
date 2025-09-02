#pragma once

#include "IoHandler.h"

class OverlappedBase : public OVERLAPPED
{
public:
	enum class IOType : uint8
	{
		Connect,
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
};
