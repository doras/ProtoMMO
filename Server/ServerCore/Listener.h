#pragma once

#include <WinSock2.h>
#include "IoHandler.h"
#include "NetAddress.h"
#include "Overlapped.h"

class Listener : public IIoHandler
{
public:
	Listener() = default;
	~Listener();

	bool Start(const NetAddress& address);
	void CloseSocket();

	virtual void OnIoCompleted(class OverlappedBase* overlapped, uint32 numOfBytes) override;
	virtual HANDLE GetHandle() const override;

	void PostAccept(AcceptOverlapped* overlapped);
	void ProcessAccept(AcceptOverlapped* overlapped);

private:
	using AcceptOverlappedUniquePtr = std::unique_ptr<AcceptOverlapped, Deleter<AcceptOverlapped, DEFAULT_ALLOCATOR>>;
	SOCKET _socket = INVALID_SOCKET;
	Vector<AcceptOverlappedUniquePtr> _acceptOverlappeds;
};

