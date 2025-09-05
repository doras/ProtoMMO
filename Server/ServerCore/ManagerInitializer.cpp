#include "pch.h"
#include "ManagerInitializer.h"
#include "ThreadManager.h"
#include "PoolAllocator.h"
#include "SocketUtils.h"

ThreadManager* GThreadManager = nullptr;
PoolAllocator* GPoolAllocator = nullptr;
StompAllocator* GStompAllocator = nullptr;

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
		if (!GStompAllocator)
		{
			GStompAllocator = new StompAllocator();
		}
		SocketUtils::Init();
	}

	~ManagerInitializer()
	{
		SocketUtils::Cleanup();
		if (GStompAllocator)
		{
			delete GStompAllocator;
			GStompAllocator = nullptr;
		}
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
