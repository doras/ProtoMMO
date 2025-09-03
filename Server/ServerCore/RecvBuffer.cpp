#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize)
	: _bufferSize(bufferSize)
{
	_buffer.resize(bufferSize);
}

void RecvBuffer::CleanIfEmpty()
{
	if (GetDataSize() == 0)
	{
		_readPos = 0;
		_writePos = 0;
	}
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes <= 0 || numOfBytes > GetFreeSize())
		return false;

	_writePos = (_writePos + numOfBytes) % _bufferSize;

	return true;
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes <= 0 || numOfBytes > GetDataSize())
		return false;

	_readPos = (_readPos + numOfBytes) % _bufferSize;

	return true;
}
