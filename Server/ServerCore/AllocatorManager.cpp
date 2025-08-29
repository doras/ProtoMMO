#include "pch.h"
#include "AllocatorManager.h"
#include "Allocator.h"
#include "PoolAllocator.h"

AllocatorManager::AllocatorManager()
{
	normalAllocator = std::make_unique<NormalAllocator>();
	stompAllocator = std::make_unique<StompAllocator>();
	poolAllocator = std::make_unique<PoolAllocator>();

	defaultAllocator = DEFAULT_ALLOCATOR.get();
}

AllocatorManager::~AllocatorManager()
{
}
