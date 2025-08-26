#include "pch.h"
#include "Allocator.h"

void* NormalAllocator::Allocate(size_t size)
{
	return ::malloc(size);
}

void NormalAllocator::Deallocate(void* ptr)
{
	::free(ptr);
}
