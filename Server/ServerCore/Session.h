#pragma once
#include "IoHandler.h"
#include "NetAddress.h"
#include "Overlapped.h"
#include "RecvBuffer.h"

class Session : public IIoHandler
{
	USE_LOCK(LockLevelInternal::Session)

	enum class ConnectionState : uint8
	{
		Disconnected,
		Connecting,
		Connected,
		Disconnecting,
	};

	friend class Listener;

	static constexpr int32 RECV_BUFFER_SIZE = 0x10000;

public:
	Session();
	virtual ~Session();

public:
	void				Send(const BYTE* data, int32 length);
	bool				Connect(); // Use only in client sessions
	void				Disconnect(const wchar_t* reason = nullptr);

	void				SetNetAddress(const NetAddress& addr) { _netAddress = addr; }
	const NetAddress&	GetNetAddress() const { return _netAddress; }
	NetAddress&			GetNetAddress() { return _netAddress; }
	ConnectionState		GetConnectionState() const { return _connectionState.load(); }
	void				SetService(ServicePtr service) { _service = service; }
	ServicePtr			GetService() const { return _service.lock(); }
	SessionPtr			GetSessionPtr() { return std::static_pointer_cast<Session>(shared_from_this()); }

private:
	virtual void OnIoCompleted(class OverlappedBase* overlapped, uint32 numOfBytes, int32 errorCode) override;
	virtual HANDLE GetHandle() const override;
	SOCKET GetSocket() const { return _socket; }

	/* IO Operations */
	bool PostConnect(); // Use only in client sessions
	bool PostDisconnect();
	void PostRecv();
	void PostSend(SendOverlapped* overlapped);

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(uint32 numOfBytes);
	void ProcessSend(SendOverlapped* overlapped, uint32 numOfBytes);

	void HandleError(int32 errorCode);

protected:
	/* Override these for custom behavior in derived classes */
	virtual void OnConnected() {}
	virtual void OnDisconnected() {}
	virtual int32 OnRecv(const RecvBuffer& buffer) { return buffer.GetDataSize(); }
	virtual void OnSend(int32 numOfBytes) {}

private:
	SOCKET					_socket = INVALID_SOCKET;
	NetAddress				_netAddress = {};
	Atomic<ConnectionState> _connectionState = { ConnectionState::Disconnected };
	std::weak_ptr<Service>	_service;

	RecvBuffer				_recvBuffer = { RECV_BUFFER_SIZE };

private:
	/* Reusable overlapped structures for async IO */
	ConnectOverlapped		_connectOverlapped;
	DisconnectOverlapped	_disconnectOverlapped;
	RecvOverlapped			_recvOverlapped;
};

