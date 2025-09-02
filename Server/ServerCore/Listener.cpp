#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpCore.h"
#include "Overlapped.h"
#include "Session.h"

Listener::~Listener()
{
	CloseSocket();
}

bool Listener::Start(const NetAddress& address)
{
	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
	{
		return false;
	}

	if (GIocpCore.Associate(shared_from_this()) == false)
	{
		return false;
	}

	if (SocketUtils::SetReuseAddr(_socket, true) == false)
	{
		return false;
	}

	if (SocketUtils::SetLinger(_socket, false, 0) == false)
	{
		return false;
	}

	if (SocketUtils::Bind(_socket, address) == false)
	{
		return false;
	}

	if (SocketUtils::Listen(_socket) == false)
	{
		return false;
	}

	// Pre-post AcceptEx
	// Post multiple AcceptEx to improve performance
	const int32 prePostCount = 5;
	for (int32 i = 0; i < prePostCount; ++i)
	{
		AcceptOverlappedUniquePtr overlapped = MakeUnique<AcceptOverlapped>();
		overlapped->owner = shared_from_this();
		PostAccept(overlapped.get());
		_acceptOverlappeds.push_back(std::move(overlapped));
	}

	return false;
}

void Listener::CloseSocket()
{
	if (_socket != INVALID_SOCKET)
	{
		SocketUtils::CloseSocket(_socket);
	}
}

void Listener::OnIoCompleted(OverlappedBase* overlapped, uint32 numOfBytes)
{
	ASSERT_CRASH(overlapped->type == OverlappedBase::IOType::Accept, "Invalid overlapped type");
	ProcessAccept(static_cast<AcceptOverlapped*>(overlapped));
}

HANDLE Listener::GetHandle() const
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::PostAccept(AcceptOverlapped* overlapped)
{
	SessionPtr session = MakeShared<Session>();

	overlapped->Init();
	overlapped->session = session;
	DWORD bytesReceived = 0;
	BOOL result = SocketUtils::AcceptEx(
		_socket,
		session->GetSocket(),
		session->_recvBuffer, // TEMP
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&bytesReceived,
		reinterpret_cast<LPOVERLAPPED>(overlapped));

	if (result == FALSE)
	{
		int32 err = ::WSAGetLastError();
		if (err != ERROR_IO_PENDING)
		{
			// TODO: Log Error
			PostAccept(overlapped); // Retry
		}
	}
}

#include <iostream> // TEMP
void Listener::ProcessAccept(AcceptOverlapped* overlapped)
{
	// Successfully accepted a connection
	SessionPtr session = overlapped->session;

	if (SocketUtils::SetUpdateAcceptContext(session->GetSocket(), _socket) == false)
	{
		PostAccept(overlapped); // Retry
		return;
	}

	NetAddress netAddr;
	if (SocketUtils::GetPeerName(session->GetSocket(), netAddr) == false)
	{
		PostAccept(overlapped); // Retry
		return;
	}

	session->SetNetAddress(netAddr);
	std::wcout << "Accepted connection from " 
		<< netAddr.GetIP() << ":" << netAddr.GetPort() << std::endl;

	PostAccept(overlapped); // Post another AcceptEx
}
