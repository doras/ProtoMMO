#pragma once

#include "RefCounting.h"

class SendBuffer : public RefCounted
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer() = default;

	int32 GetCapacity() const { return static_cast<int32>(_buffer.size()); }
	int32 GetDataSize() const { return _dataSize; }
	BYTE* GetBufferPtr() { return _buffer.data(); }
	const BYTE* GetBufferPtr() const { return _buffer.data(); }

	void Write(const void* data, int32 length);

private:
	Vector<BYTE> _buffer;
	int32 _dataSize = 0;
};

using SendBufferPtr = IntrusivePtr<class SendBuffer>;
