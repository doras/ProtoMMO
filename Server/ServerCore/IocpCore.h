#pragma once

class IocpCore : public std::enable_shared_from_this<IocpCore>
{
public:
	IocpCore();
	~IocpCore();

	// Associate an IO handler (like a socket) with the IOCP
	bool Associate(IIoHandlerPtr handler) const;

	// Poll for completed IO events and dispatch them to their handlers
	bool PollAndDispatch(uint32 timeoutMs = INFINITE) const;

private:
	HANDLE _iocpHandle;
};
