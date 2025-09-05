#pragma once

constexpr size_t SLIST_ALIGNMENT = 16;

struct alignas(SLIST_ALIGNMENT) MemoryHeader : public SLIST_ENTRY
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(void* ptr, int32 size)
	{
		new (ptr) MemoryHeader(size);
#ifdef _STOMP
		int32 dataSize = size - sizeof(MemoryHeader);
		int32 padding = (SLIST_ALIGNMENT - (dataSize % SLIST_ALIGNMENT)) % SLIST_ALIGNMENT;
		uint64 address = reinterpret_cast<uint64>(ptr);
		return reinterpret_cast<void*>(address + sizeof(MemoryHeader) + padding);
#else
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr);
		return header + 1;
#endif // _STOMP
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
#ifdef _STOMP
		uint64 address = reinterpret_cast<uint64>(ptr);
		address = address - (address % SLIST_ALIGNMENT);
		return reinterpret_cast<MemoryHeader*>(address) - 1;
#else
		return reinterpret_cast<MemoryHeader*>(ptr) - 1;
#endif // _STOMP
	}

	int32 allocSize;
};

class alignas(SLIST_ALIGNMENT) MemoryPool
{
public:
	MemoryPool(int32 blockSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader*	Pop();

private:
	SLIST_HEADER _header;
	int32 _blockSize;
	Atomic<int32> _usedCount = { 0 };
	Atomic<int32> _allocCount = { 0 };
};
