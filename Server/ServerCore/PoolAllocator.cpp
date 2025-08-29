#include "pch.h"
#include "PoolAllocator.h"
#include "MemoryPool.h"

int32 PoolAllocator::sPoolSizes[] = { 32, 64, 128, 256, 512, 1024, 2048, PoolAllocator::MAX_BLOCK_SIZE };
int32 PoolAllocator::sLookupTable[PoolAllocator::MAX_BLOCK_SIZE + 1] = { 0 };

PoolAllocator::PoolAllocator()
{
	int32 lookupIndex = 0;
	_pools.resize(NUM_POOLS);

	for (int32 i = 0; i < NUM_POOLS; ++i)
	{
		_pools[i] = new (::_aligned_malloc(sizeof(MemoryPool), SLIST_ALIGNMENT)) MemoryPool(sPoolSizes[i]);
		while (lookupIndex <= sPoolSizes[i])
		{
			sLookupTable[lookupIndex] = i;
			++lookupIndex;
		}
	}
}

PoolAllocator::~PoolAllocator()
{
	for (MemoryPool* pool : _pools)
	{
		pool->~MemoryPool();
		::_aligned_free(pool);
	}

	_pools.clear();
}

void* PoolAllocator::Allocate(size_t size)
{
	if (size == 0)
	{
		return nullptr;
	}

	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);

	if (allocSize > MAX_BLOCK_SIZE)
	{
		header = static_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
#pragma warning(push)
#pragma warning(disable: 6305) // Suppress C6305 for this block
        int32 poolIndex = sLookupTable[allocSize];
#pragma warning(pop)
        header = _pools[poolIndex]->Pop();
	}

	return MemoryHeader::AttachHeader(header, allocSize);
}

void PoolAllocator::Deallocate(void* ptr)
{
	if (ptr == nullptr)
	{
		return;
	}
	
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
	const int32 allocSize = header->allocSize;

	ASSERT_CRASH(allocSize > 0);

	if (allocSize > MAX_BLOCK_SIZE)
	{
		::_aligned_free(header);
	}
	else
	{
		const int32 poolIndex = sLookupTable[allocSize];
		_pools[poolIndex]->Push(header);
	}
}
