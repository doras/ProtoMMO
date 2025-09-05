#include "pch.h"
#include "Allocator.h"

void* NormalAllocator::Allocate(NormalAllocator*, size_t size)
{
	return ::malloc(size);
}

void NormalAllocator::Deallocate(NormalAllocator*, void* ptr)
{
	::free(ptr);
}

StompAllocator::StompAllocator()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	_pageSize = sysInfo.dwPageSize;
}

void* StompAllocator::Allocate(StompAllocator* allocator, size_t size)
{
	if (allocator == nullptr || size == 0)
	{
		return nullptr;
	}

	size_t allocSize = ((size + allocator->_pageSize - 1) / allocator->_pageSize) * allocator->_pageSize;
	// Allocate an extra page for guard
	void* baseAddr = ::VirtualAlloc(nullptr, allocSize + allocator->_pageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!baseAddr)
	{
		return nullptr;
	}

	int8* endAddr = static_cast<int8*>(baseAddr) + allocSize;

	DWORD oldProtect;
	// Set the extra page as no access
	::VirtualProtect(endAddr, allocator->_pageSize, PAGE_NOACCESS, &oldProtect);

	return endAddr - size;
}

void StompAllocator::Deallocate(StompAllocator* allocator, void* ptr)
{
	if (allocator == nullptr || ptr == nullptr)
	{
		return;
	}
	
	size_t address = reinterpret_cast<size_t>(ptr);
	size_t baseAddr = address - (address % allocator->_pageSize);
	::VirtualFree(reinterpret_cast<void*>(baseAddr), 0, MEM_RELEASE);
}
