#include "pch.h"
#include "IocpCore.h"
#include "IoHandler.h"
#include "Overlapped.h"

#include "WinSock2.h"

// TEMP
IocpCore GIocpCore;

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	ASSERT_CRASH(_iocpHandle != NULL, "Failed to create IOCP");
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Associate(IIoHandlerPtr handler) const
{
	return ::CreateIoCompletionPort(handler->GetHandle(), _iocpHandle, 0, 0) != NULL;
}

bool IocpCore::PollAndDispatch(uint32 timeoutMs) const
{
	DWORD numOfBytes = 0;
	ULONG_PTR completionKey = 0;
	OverlappedBase* overlapped = nullptr;

	BOOL result = ::GetQueuedCompletionStatus(
		_iocpHandle,
		&numOfBytes,
		&completionKey,
		reinterpret_cast<LPOVERLAPPED*>(&overlapped),
		timeoutMs);

	if (result)
	{
		IIoHandlerPtr owner;
		if (overlapped == nullptr || (owner = overlapped->owner) == nullptr)
		{
			// TODO: Log Error
			return false;
		}

		owner->OnIoCompleted(overlapped, numOfBytes);
	}
	else
	{
		int32 err = ::WSAGetLastError();
		if (err != WAIT_TIMEOUT)
		{
			// TODO: Log Error
		}
		
		return false;
	}

	return true;
}
