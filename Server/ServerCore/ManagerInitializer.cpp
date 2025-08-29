#include "pch.h"
#include "ManagerInitializer.h"
#include "ThreadManager.h"
#include "AllocatorManager.h"

ThreadManager* GThreadManager = nullptr;
AllocatorManager* GAllocatorManager = nullptr;

class ManagerInitializer
{
public:
	ManagerInitializer()
	{
		if (!GThreadManager)
		{
			GThreadManager = new ThreadManager();
		}
		if (!GAllocatorManager)
		{
			GAllocatorManager = new AllocatorManager();
		}
	}

	~ManagerInitializer()
	{
		if (GAllocatorManager)
		{
			delete GAllocatorManager;
			GAllocatorManager = nullptr;
		}
		if (GThreadManager)
		{
			delete GThreadManager;
			GThreadManager = nullptr;
		}
	}
} GManagerInitializer;
