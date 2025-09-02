#pragma once

class IIoHandler : public std::enable_shared_from_this<IIoHandler>
{
public:
	virtual void OnIoCompleted(class OverlappedBase* overlapped, uint32 numOfBytes) = 0;
	virtual HANDLE GetHandle() const = 0;
};
