#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::CloseSocket(_socket);
}

void Session::OnIoCompleted(OverlappedBase* overlapped, uint32 numOfBytes)
{
	// TODO
}

HANDLE Session::GetHandle() const
{
	return reinterpret_cast<HANDLE>(_socket);
}
