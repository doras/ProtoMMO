#pragma once

#include <memory>
#include "Allocator.h"
#include "PoolAllocator.h"

/*-----------------
	New & Delete
------------------*/

template<typename T, typename Allocator, typename... Args>
T* CoreNew(Allocator* allocator, Args&&... args)
{
	void* ptr = Allocator::Allocate(allocator, sizeof(T));
	return ::new (ptr) T(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
T* CoreNew(Args&&... args)
{
	return CoreNew<T, DEFAULT_ALLOCATOR>(DEFAULT_ALLOCATOR_OBJECT, std::forward<Args>(args)...);
}

template<typename T, typename Allocator>
void CoreDelete(Allocator* allocator, T* ptr)
{
	if (ptr)
	{
		ptr->~T();
		Allocator::Deallocate(allocator, ptr);
	}
}

template<typename T>
void CoreDelete(T* ptr)
{
	CoreDelete<T, DEFAULT_ALLOCATOR>(DEFAULT_ALLOCATOR_OBJECT, ptr);
}

/*-----------------
	Smart Pointer
------------------*/

template<typename T, typename Allocator>
struct Deleter
{
	Allocator* allocator;
	void operator()(T* ptr) const
	{
		CoreDelete<T, Allocator>(allocator, ptr);
	}
};

template<typename T, typename Allocator, typename... Args>
std::unique_ptr<T, Deleter<T, Allocator>> MakeUnique(Allocator* allocator, Args&&... args)
{
    return std::unique_ptr<T, Deleter<T, Allocator>>(
        CoreNew<T, Allocator>(allocator, std::forward<Args>(args)...),
        Deleter<T, Allocator>{ allocator }
    );
}

template<typename T, typename... Args>
std::unique_ptr<T, Deleter<T, DEFAULT_ALLOCATOR>> MakeUnique(Args&&... args)
{
	return MakeUnique<T, DEFAULT_ALLOCATOR>(DEFAULT_ALLOCATOR_OBJECT, std::forward<Args>(args)...);
}

template<typename T, typename Allocator, typename... Args>
std::shared_ptr<T> MakeShared(Allocator* allocator, Args&&... args)
{
	return std::shared_ptr<T>(CoreNew<T, Allocator>(allocator, std::forward<Args>(args)...), Deleter<T, Allocator>{ allocator });
}

template<typename T, typename... Args>
std::shared_ptr<T> MakeShared(Args&&... args)
{
	return MakeShared<T, DEFAULT_ALLOCATOR>(DEFAULT_ALLOCATOR_OBJECT, std::forward<Args>(args)...);
}
