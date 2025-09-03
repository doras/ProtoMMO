#pragma once

class IIoHandler : public std::enable_shared_from_this<IIoHandler>
{
public:
	virtual void OnIoCompleted(class OverlappedBase* overlapped, uint32 numOfBytes, int32 errorCode) = 0; // errorCode is 0 if success, otherwise the error code
	virtual HANDLE GetHandle() const = 0;
};
