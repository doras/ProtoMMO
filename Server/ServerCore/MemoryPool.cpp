#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 blockSize) : _blockSize(blockSize)
{
	::InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
	{
		::_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->allocSize = 0;
	::InterlockedPushEntrySList(&_header, ptr);
	_usedCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));
	
	if (memory == nullptr)
	{
		memory = static_cast<MemoryHeader*>(::_aligned_malloc(_blockSize, SLIST_ALIGNMENT));
		_allocCount.fetch_add(1);
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
	}

	_usedCount.fetch_add(1);
	
	return memory;
}
