#pragma once

#include <thread>
#include <functional>

#include "Lock.h"

class ThreadManager
{
	USE_LOCK(LockLevelInternal::ThreadManager);

public:
	ThreadManager();
	~ThreadManager();

	void		Launch(std::function<void(void)> callback);
	void		Join();

	static void	InitTLS();
	static void DestroyTLS();

private:
	std::vector<std::thread>	_threads;
};

