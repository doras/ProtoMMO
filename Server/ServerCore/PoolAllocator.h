#pragma once

class PoolAllocator
{
private:
	enum
	{
		MAX_BLOCK_SIZE = 4096,
		NUM_POOLS = 8,
	};

	static int32 sPoolSizes[NUM_POOLS];
	static int32 sLookupTable[MAX_BLOCK_SIZE + 1];

public:
	PoolAllocator();
	~PoolAllocator();

	static void* Allocate(PoolAllocator* allocator, size_t size);
	static void  Deallocate(PoolAllocator* allocator, void* ptr);

private:
	std::vector<class MemoryPool*> _pools;
};

