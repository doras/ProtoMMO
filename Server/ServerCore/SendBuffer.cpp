#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int32 bufferSize)
{
	_buffer.resize(bufferSize);
}

void SendBuffer::Write(const void* data, int32 length)
{
	ASSERT_CRASH(length > 0 && length <= GetCapacity());
	::memcpy(_buffer.data(), data, length);
	_dataSize = length;
}
