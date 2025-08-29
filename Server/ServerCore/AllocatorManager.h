#pragma once

class AllocatorManager
{
public:
	AllocatorManager();
	~AllocatorManager();

	std::unique_ptr<class NormalAllocator> normalAllocator;
	std::unique_ptr<class StompAllocator> stompAllocator;
	std::unique_ptr<class PoolAllocator> poolAllocator;

	class IAllocator* defaultAllocator;
};
