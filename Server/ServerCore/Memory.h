#pragma once

#include "AllocatorManager.h"
#include "Allocator.h"

template<typename T, typename... Args>
T* CoreNewWithAllocator(IAllocator* allocator, Args&&... args)
{
	void* ptr = allocator->Allocate(sizeof(T));
	return ::new (ptr) T(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
T* CoreNew(Args&&... args)
{
	return CoreNewWithAllocator<T>(GAllocatorManager->defaultAllocator, std::forward<Args>(args)...);
}

template<typename T>
void CoreDeleteWithAllocator(IAllocator* allocator, T* ptr)
{
	if (ptr)
	{
		ptr->~T();
		allocator->Deallocate(ptr);
	}
}

template<typename T>
void CoreDelete(T* ptr)
{
	CoreDeleteWithAllocator<T>(GAllocatorManager->defaultAllocator, ptr);
}
