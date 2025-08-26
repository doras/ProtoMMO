#pragma once

#include "Allocator.h"

class MemoryManager
{
public:
	MemoryManager() = default;

	NormalAllocator normalAllocator;
	StompAllocator stompAllocator;

	IAllocator& defaultAllocator = DEFAULT_ALLOCATOR;
};

template<typename T, typename... Args>
T* CoreNewWithAllocator(IAllocator& allocator, Args&&... args)
{
	void* ptr = allocator.Allocate(sizeof(T));
	return ::new (ptr) T(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
T* CoreNew(Args&&... args)
{
	return CoreNewWithAllocator<T>(GMemoryManager->defaultAllocator, std::forward<Args>(args)...);
}

template<typename T>
void CoreDeleteWithAllocator(IAllocator& allocator, T* ptr)
{
	if (ptr)
	{
		ptr->~T();
		allocator.Deallocate(ptr);
	}
}

template<typename T>
void CoreDelete(T* ptr)
{
	CoreDeleteWithAllocator<T>(GMemoryManager->defaultAllocator, ptr);
}
