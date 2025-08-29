#pragma once

constexpr size_t SLIST_ALIGNMENT = 16;

struct alignas(SLIST_ALIGNMENT) MemoryHeader : public SLIST_ENTRY
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(void* ptr, int32 size)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr);
		new (header) MemoryHeader(size);
		return header + 1;
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		return reinterpret_cast<MemoryHeader*>(ptr) - 1;
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
};
