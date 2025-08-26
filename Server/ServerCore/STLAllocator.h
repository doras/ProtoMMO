#pragma once

#include "Memory.h"

template<typename T>
class STLAllocator
{
public:
	using value_type = T;

	STLAllocator() = default;

	template<typename U>
	STLAllocator(const STLAllocator<U>&) {}

	T* allocate(size_t n)
	{
		size_t size = n * sizeof(T);
		return static_cast<T*>(GMemoryManager->defaultAllocator.Allocate(size));
	}

	void deallocate(T* ptr, size_t)
	{
		GMemoryManager->defaultAllocator.Deallocate(ptr);
	}
};
