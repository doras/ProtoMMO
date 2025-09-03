#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include <iostream> // TEMP

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::CloseSocket(_socket);
}

void Session::Send(const BYTE* data, int32 length)
{
	if (GetConnectionState() != ConnectionState::Connected)
		return;

	// TEMP
	SendOverlapped* sendOverlapped = CoreNew<SendOverlapped>();
	sendOverlapped->buffer.resize(length);
	::memcpy(sendOverlapped->buffer.data(), data, length);

	PostSend(sendOverlapped);
}

bool Session::Connect()
{
	return PostConnect();
}

void Session::Disconnect(const wchar_t* reason)
{
	// Only disconnect if currently connected
	if (GetConnectionState() != ConnectionState::Connected)
		return;

	// TEMP: Log reason
	if (reason)
		std::wcout << L"Disconnecting: " << reason << std::endl;
	else
		std::wcout << L"Disconnecting" << std::endl;

	if (PostDisconnect() == false)
	{
		// TEMP: LOG Error
		std::cout << "PostDisconnect failed" << std::endl;
	}
}

void Session::OnIoCompleted(OverlappedBase* overlapped, uint32 numOfBytes, int32 errorCode)
{
	if (errorCode != 0)
	{
		if (overlapped->owner)
		{
			overlapped->owner = nullptr; // DecRef this as the IO is complete (with error)
		}
		HandleError(errorCode);
		return;
	}

	switch (overlapped->type)
	{
	case OverlappedBase::IOType::Connect:
		ProcessConnect();
		break;
	case OverlappedBase::IOType::Disconnect:
		ProcessDisconnect();
		break;
	case OverlappedBase::IOType::Recv:
 		ProcessRecv(numOfBytes);
		break;
	case OverlappedBase::IOType::Send:
		ProcessSend(static_cast<SendOverlapped*>(overlapped), numOfBytes);
		break;
	default:
		// TODO: Log Error
		break;
	}
}

HANDLE Session::GetHandle() const
{
	return reinterpret_cast<HANDLE>(_socket);
}

bool Session::PostConnect()
{
	if (GetService()->GetType() != Service::Type::Client)
		return false; // Only for client sessions

	ConnectionState expected = ConnectionState::Disconnected;
	if (_connectionState.compare_exchange_strong(expected, ConnectionState::Connecting) == false)
		return false; // Not in Disconnected state

	if (SocketUtils::BindAnyAddr(_socket, 0) == false)
	{
		_connectionState.store(ConnectionState::Disconnected);
		return false;
	}

	_connectOverlapped.Init();
	_connectOverlapped.owner = shared_from_this(); // IncRef this (to ensure it lives until the IO completes)

	DWORD bytesSent = 0;
	SOCKADDR_IN addr = GetService()->GetAddress().GetSockAddr();
	BOOL result = SocketUtils::ConnectEx(
		_socket,
		reinterpret_cast<SOCKADDR*>(&addr),
		sizeof(addr),
		nullptr,
		0,
		&bytesSent,
		&_connectOverlapped);
	if (result == FALSE)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != ERROR_IO_PENDING)
		{
			_connectOverlapped.owner = nullptr; // DecRef this if error
			_connectionState.store(ConnectionState::Disconnected);
			return false;
		}
	}
}

bool Session::PostDisconnect()
{
	ConnectionState expected = ConnectionState::Connected;
	if (_connectionState.compare_exchange_strong(expected, ConnectionState::Disconnecting) == false)
		return false; // Not in Connected state

	_disconnectOverlapped.Init();
	_disconnectOverlapped.owner = shared_from_this(); // IncRef this (to ensure it lives until the IO completes)

	BOOL result = SocketUtils::DisconnectEx(_socket, &_disconnectOverlapped, TF_REUSE_SOCKET, 0);
	if (result == FALSE)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != ERROR_IO_PENDING)
		{
			_disconnectOverlapped.owner = nullptr; // DecRef this if error
			_connectionState.store(ConnectionState::Connected);
			return false;
		}
	}

	return true;
}

void Session::PostRecv()
{
	if (GetConnectionState() != ConnectionState::Connected)
		return;

	_recvOverlapped.Init();
	_recvOverlapped.owner = shared_from_this(); // IncRef this (to ensure it lives until the IO completes)

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<CHAR*>(_recvBuffer);
	wsaBuf.len = countof(_recvBuffer);
	
	DWORD bytesReceived = 0;
	DWORD flags = 0;
	int32 result = ::WSARecv(_socket, &wsaBuf, 1, &bytesReceived, &flags, &_recvOverlapped, nullptr);
	if (result == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvOverlapped.owner = nullptr; // DecRef this if error
		}
	}
}

void Session::PostSend(SendOverlapped* overlapped)
{
	if (GetConnectionState() != ConnectionState::Connected)
	{
		CoreDelete(overlapped);
		return;
	}

	overlapped->owner = shared_from_this(); // IncRef this (to ensure it lives until the IO completes)

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<CHAR*>(overlapped->buffer.data());
	wsaBuf.len = static_cast<ULONG>(overlapped->buffer.size());

	DWORD bytesSent = 0;
	int32 result;
	{
		WRITE_LOCK;
		result = ::WSASend(_socket, &wsaBuf, 1, &bytesSent, 0, overlapped, nullptr);
	}
	if (result == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			overlapped->owner = nullptr; // DecRef this if error
			CoreDelete(overlapped);
		}
	}
}

void Session::ProcessConnect()
{
	_connectOverlapped.owner = nullptr; // DecRef this as the IO is complete

	_connectionState.store(ConnectionState::Connected);

	// Add to the service's session list
	GetService()->AddSession(GetSessionPtr());

	// Notify derived class
	OnConnected();

	// Start receiving data
	PostRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectOverlapped.owner = nullptr; // DecRef this as the IO is complete

	_connectionState.store(ConnectionState::Disconnected);

	// Remove from the service's session list
	GetService()->RemoveSession(GetSessionPtr());

	// Notify derived class
	OnDisconnected();
}

void Session::ProcessRecv(uint32 numOfBytes)
{
	_recvOverlapped.owner = nullptr; // DecRef this as the IO is complete

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	// Notify derived class
	OnRecv(_recvBuffer, numOfBytes);

	// Post another recv
	PostRecv();
}

void Session::ProcessSend(SendOverlapped* overlapped, uint32 numOfBytes)
{
	overlapped->owner = nullptr; // DecRef this as the IO is complete
	CoreDelete(overlapped);

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// Notify derived class
	OnSend(numOfBytes);
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
	case ERROR_NETNAME_DELETED:
		Disconnect(L"Connection Error");
		break;
	default:
		// TODO: Log other errors
		std::cout << "WSA Error: " << errorCode << std::endl;
		break;
	}
}
