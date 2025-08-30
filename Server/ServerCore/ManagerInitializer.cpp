#include "pch.h"
#include "ManagerInitializer.h"
#include "ThreadManager.h"
#include "PoolAllocator.h"
#include "SocketUtils.h"

ThreadManager* GThreadManager = nullptr;
PoolAllocator* GPoolAllocator = nullptr;

class ManagerInitializer
{
public:
	ManagerInitializer()
	{
		if (!GThreadManager)
		{
			GThreadManager = new ThreadManager();
		}
		if (!GPoolAllocator)
		{
			GPoolAllocator = new PoolAllocator();
		}
		SocketUtils::Init();
	}

	~ManagerInitializer()
	{
		SocketUtils::Cleanup();
		if (GPoolAllocator)
		{
			delete GPoolAllocator;
			GPoolAllocator = nullptr;
		}
		if (GThreadManager)
		{
			delete GThreadManager;
			GThreadManager = nullptr;
		}
	}
} GManagerInitializer;
