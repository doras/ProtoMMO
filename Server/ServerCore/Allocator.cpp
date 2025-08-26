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

StompAllocator::StompAllocator()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	_pageSize = sysInfo.dwPageSize;
}

void* StompAllocator::Allocate(size_t size)
{
	size_t allocSize = ((size + _pageSize - 1) / _pageSize) * _pageSize;
	void* baseAddr = ::VirtualAlloc(nullptr, allocSize + _pageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!baseAddr)
	{
		return nullptr;
	}

	DWORD oldProtect;
	::VirtualProtect(static_cast<int8*>(baseAddr) + _pageSize, _pageSize, PAGE_NOACCESS, &oldProtect);

	return static_cast<int8*>(baseAddr) + (allocSize - size);
}

void StompAllocator::Deallocate(void* ptr)
{
	if (!ptr)
	{
		return;
	}
	
	size_t address = reinterpret_cast<size_t>(ptr);
	size_t baseAddr = address - (address % _pageSize);
	::VirtualFree(reinterpret_cast<void*>(baseAddr), 0, MEM_RELEASE);
}
