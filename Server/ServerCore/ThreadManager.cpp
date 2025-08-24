#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"

using std::thread;

ThreadManager::ThreadManager()
{
	// Initialize main thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
	DestroyTLS(); // for main thread
}

void ThreadManager::Launch(std::function<void(void)> callback)
{
	WRITE_LOCK;
	
	_threads.push_back(thread([this, callback]() {
		InitTLS();
		callback();
		DestroyTLS();
	}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}
