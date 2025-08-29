#pragma once

#include "Allocator.h"

class PoolAllocator : public IAllocator
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

	virtual void* Allocate(size_t size) override;
	virtual void  Deallocate(void* ptr) override;

private:
	std::vector<class MemoryPool*> _pools;
};

