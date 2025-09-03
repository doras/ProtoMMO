#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpCore.h"
#include "Overlapped.h"
#include "Session.h"
#include "Service.h"

Listener::~Listener()
{
	CloseSocket();
}

bool Listener::Start(ServerServicePtr service)
{
	if (service == nullptr)
		return false;
	_service = service;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	if (service->GetIocpCore()->Associate(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddr(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, false, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, service->GetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	// Pre-post AcceptEx
	// Post multiple AcceptEx to improve performance
	const int32 prePostCount = service->GetMaxSessionCount();
	for (int32 i = 0; i < prePostCount; ++i)
	{
		AcceptOverlappedUniquePtr overlapped = MakeUnique<AcceptOverlapped>();
		overlapped->owner = shared_from_this();
		PostAccept(overlapped.get());
		_acceptOverlappeds.push_back(std::move(overlapped));
	}

	return true;
}

void Listener::CloseSocket()
{
	if (_socket != INVALID_SOCKET)
	{
		SocketUtils::CloseSocket(_socket);
	}
}

void Listener::OnIoCompleted(OverlappedBase* overlapped, uint32 numOfBytes, int32 errorCode)
{
	ASSERT_CRASH(overlapped->type == OverlappedBase::IOType::Accept, "Invalid overlapped type");

	// If error, just ignore and post another AcceptEx
	if (errorCode != 0)
	{
		PostAccept(static_cast<AcceptOverlapped*>(overlapped));
		return;
	}

	ProcessAccept(static_cast<AcceptOverlapped*>(overlapped));
}

HANDLE Listener::GetHandle() const
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::PostAccept(AcceptOverlapped* overlapped)
{
	ServerServicePtr service = _service.lock();
	if (service == nullptr)
		return;

	SessionPtr session = service->CreateSession();

	overlapped->Init();
	overlapped->session = session;
	DWORD bytesReceived = 0;
	BOOL result = SocketUtils::AcceptEx(
		_socket,
		session->GetSocket(),
		session->_recvBuffer.GetWritePtr(),
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

	session->ProcessConnect(); // Notify the session that it is connected

	PostAccept(overlapped); // Post another AcceptEx
}
