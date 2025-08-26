#include "pch.h"
#include "ManagerInitializer.h"
#include "ThreadManager.h"
#include "Memory.h"

ThreadManager* GThreadManager = nullptr;
MemoryManager* GMemoryManager = nullptr;

class ManagerInitializer
{
public:
	ManagerInitializer()
	{
		if (!GThreadManager)
		{
			GThreadManager = new ThreadManager();
		}
		if (!GMemoryManager)
		{
			GMemoryManager = new MemoryManager();
		}
	}

	~ManagerInitializer()
	{
		if (GMemoryManager)
		{
			delete GMemoryManager;
			GMemoryManager = nullptr;
		}
		if (GThreadManager)
		{
			delete GThreadManager;
			GThreadManager = nullptr;
		}
	}
} GManagerInitializer;
