#pragma once

#include "Types.h"
#include "MemoryPool.h"

template<typename T>
class ObjectPoolAllocator
{
public:
	static void* Allocate(ObjectPoolAllocator<T>*, size_t size)
	{
		if (size != sizeof(T))
		{
			return nullptr;
		}

		return MemoryHeader::AttachHeader(sPool.Pop(), sBlockSize);
	}

	static void Deallocate(ObjectPoolAllocator<T>*, void* ptr)
	{
		if (!ptr)
		{
			return;
		}

		sPool.Push(MemoryHeader::DetachHeader(ptr));
	}

private:
	static int32 sBlockSize;
	static MemoryPool sPool;
};

template<typename T>
int32 ObjectPoolAllocator<T>::sBlockSize = sizeof(T) + sizeof(MemoryHeader);

template<typename T>
MemoryPool ObjectPoolAllocator<T>::sPool(ObjectPoolAllocator<T>::sBlockSize);
