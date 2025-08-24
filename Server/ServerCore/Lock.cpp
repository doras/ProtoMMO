#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadlockDetector.h"

Lock::Lock(uint8 level) : _level(level)
{
	InitializeSRWLock(&_lock);
}

Lock::~Lock()
{
}

void Lock::ReadLock()
{
#ifdef ENABLE_DEADLOCK_DETECTION
	LDeadlockDetector.PushLock(_level);
#endif // ENABLE_DEADLOCK_DETECTION

	AcquireSRWLockShared(&_lock);
}

void Lock::ReadUnlock()
{
#ifdef ENABLE_DEADLOCK_DETECTION
	LDeadlockDetector.PopLock(_level);
#endif // ENABLE_DEADLOCK_DETECTION
	ReleaseSRWLockShared(&_lock);
}

void Lock::WriteLock()
{
#ifdef ENABLE_DEADLOCK_DETECTION
	LDeadlockDetector.PushLock(_level);
#endif // ENABLE_DEADLOCK_DETECTION
	AcquireSRWLockExclusive(&_lock);
}

void Lock::WriteUnlock()
{
#ifdef ENABLE_DEADLOCK_DETECTION
	LDeadlockDetector.PopLock(_level);
#endif // ENABLE_DEADLOCK_DETECTION
	ReleaseSRWLockExclusive(&_lock);
}
