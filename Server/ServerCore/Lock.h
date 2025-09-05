#pragma once

enum class LockLevelInternal : uint8
{
	None = 0,
	ThreadManager,
	Service,
	GameSessionManager,
	Session,
	Max = 99,
};

class Lock
{
public:
	Lock() = delete;
	Lock(uint8 level);
	~Lock();

	void ReadLock();
	void ReadUnlock();
	void WriteLock();
	void WriteUnlock();

private:
	SRWLOCK _lock;
	uint8 _level;
};

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock) { lock.ReadLock(); }
	~ReadLockGuard() { _lock.ReadUnlock(); }

private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { lock.WriteLock(); }
	~WriteLockGuard() { _lock.WriteUnlock(); }

private:
	Lock& _lock;
};
