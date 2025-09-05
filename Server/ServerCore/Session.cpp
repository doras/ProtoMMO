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

void Session::Send(SendBufferPtr sendBuffer)
{
	if (sendBuffer == nullptr || sendBuffer->GetDataSize() == 0)
		return;

	if (GetConnectionState() != ConnectionState::Connected)
		return;

	bool needToPostSend = false;
	{
		WRITE_LOCK;

		_sendQueue.push(sendBuffer);

		if (_isSending == false)
		{
			_isSending = true;
			needToPostSend = true;
		}
	}

	if (needToPostSend)
	{
		PostSend();
	}
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
		ProcessSend(numOfBytes);
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

	WSABUF wsaBufs[2];

	wsaBufs[0].buf = reinterpret_cast<CHAR*>(_recvBuffer.GetWritePtr());
	wsaBufs[0].len = _recvBuffer.GetContiguousFreeSize();

	wsaBufs[1].buf = reinterpret_cast<CHAR*>(_recvBuffer.GetBufferPtr());
	wsaBufs[1].len = _recvBuffer.GetFreeSize() - wsaBufs[0].len;
	
	DWORD bufCount = (wsaBufs[1].len > 0 ? 2 : 1);
	DWORD bytesReceived = 0;
	DWORD flags = 0;
	int32 result = ::WSARecv(_socket, wsaBufs, bufCount, &bytesReceived, &flags, &_recvOverlapped, nullptr);
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

void Session::PostSend()
{
	if (GetConnectionState() != ConnectionState::Connected)
		return;

	_sendOverlapped.Init();
	_sendOverlapped.owner = shared_from_this(); // IncRef this (to ensure it lives until the IO completes)

	{
		WRITE_LOCK;
		while (!_sendQueue.empty())
		{
			SendBufferPtr sendBuffer = _sendQueue.front();

			// TODO: Limit the number of buffers to send at once (to avoid too large WSABUF array)
			// or limit the total size of data to send at once

			_sendQueue.pop();
			_sendOverlapped.sendBuffers.push_back(sendBuffer); // Keep a reference to the send buffer until the send is complete
		}
	}

	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendOverlapped.sendBuffers.size());
	for (SendBufferPtr& sendBuffer : _sendOverlapped.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<CHAR*>(sendBuffer->GetBufferPtr());
		wsaBuf.len = sendBuffer->GetDataSize();
		wsaBufs.push_back(wsaBuf);
	}

	DWORD bytesSent = 0;
	int32 result;
	{
		WRITE_LOCK;
		result = ::WSASend(_socket, wsaBufs.data(), wsaBufs.size(), &bytesSent, 0, &_sendOverlapped, nullptr);
	}
	if (result == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendOverlapped.owner = nullptr; // DecRef this if error
			_sendOverlapped.sendBuffers.clear(); // Release all send buffers associated with this send operation

			{
				WRITE_LOCK;
				_isSending = false;
			}
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

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"Recv Buffer Overflow");
		return;
	}

	// Notify derived class
	int32 dataSize = _recvBuffer.GetDataSize();
	int32 consumedLen = OnRecv(_recvBuffer);
	if (consumedLen < 0 || consumedLen > dataSize || _recvBuffer.OnRead(consumedLen) == false)
	{
		Disconnect(L"Recv Buffer Invalid Read");
		return;
	}

	// Clean up the buffer if it's empty for optimization
	_recvBuffer.CleanIfEmpty();

	// Post another recv
	PostRecv();
}

void Session::ProcessSend(uint32 numOfBytes)
{
	_sendOverlapped.owner = nullptr; // DecRef this as the IO is complete
	_sendOverlapped.sendBuffers.clear(); // Release all send buffers associated with this send operation

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// Notify derived class
	OnSend(numOfBytes);

	bool needToPostSend = false;
	{
		WRITE_LOCK;
		if (_sendQueue.empty())
		{
			_isSending = false;
		}
		else
		{
			needToPostSend = true;
		}
	}

	if (needToPostSend)
	{
		PostSend();
	}
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
