#include "pch.h"
#include "ManagerInitializer.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

class ManagerInitializer
{
public:
	ManagerInitializer()
	{
		if (!GThreadManager)
		{
			GThreadManager = new ThreadManager();
		}
	}

	~ManagerInitializer()
	{
		if (GThreadManager)
		{
			delete GThreadManager;
			GThreadManager = nullptr;
		}
	}
} GManagerInitializer;
