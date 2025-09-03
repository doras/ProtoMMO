#pragma once

// Circular Buffer for IOCP Recv
class RecvBuffer
{
public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer() = default;

	void CleanIfEmpty();
	bool OnWrite(int32 numOfBytes);
	bool OnRead(int32 numOfBytes);

	int32 GetBufferSize() const { return _bufferSize; }
	int32 GetDataSize() const 
	{ 
		return _writePos >= _readPos 
			? _writePos - _readPos 
			: _bufferSize - _readPos + _writePos; 
	}
	int32 GetFreeSize() const { return _bufferSize - GetDataSize(); }

	int32 GetContiguousFreeSize() const
	{
		return (_writePos >= _readPos)
			? _bufferSize - _writePos
			: _readPos - _writePos;
	}
	int32 GetContiguousDataSize() const
	{
		return (_writePos >= _readPos)
			? _writePos - _readPos
			: _bufferSize - _readPos;
	}

	BYTE* GetBufferPtr() { return _buffer.data(); }
	const BYTE* GetBufferPtr() const { return _buffer.data(); }
	BYTE* GetWritePtr() { return &_buffer[_writePos]; }
	const BYTE* GetReadPtr() const { return &_buffer[_readPos]; }

	bool CanWriteContiguously(int32 numOfBytes) const { return (numOfBytes > 0 && numOfBytes <= GetContiguousFreeSize()); }
	bool CanReadContiguously(int32 numOfBytes) const { return (numOfBytes > 0 && numOfBytes <= GetContiguousDataSize()); }

private:
	int32 _bufferSize;
	int32 _readPos = 0;
	int32 _writePos = 0;
	Vector<BYTE> _buffer;
};

