#pragma once

class NormalAllocator
{
public:
	static void* Allocate(NormalAllocator*, size_t size);
	static void  Deallocate(NormalAllocator*, void* ptr);
};

class StompAllocator
{
public:
	StompAllocator();

	static void* Allocate(StompAllocator* allocator, size_t size);
	static void  Deallocate(StompAllocator* allocator, void* ptr);

private:
	size_t _pageSize = 0;
};
