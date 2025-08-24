#include "pch.h"
#include "DeadlockDetector.h"
#include "Lock.h"

void DeadlockDetector::PushLock(uint8 level)
{
	for (uint8 acquiredLevel : _acquiredLockLevels)
	{
		// Check for locking strategy violation
		ASSERT_CRASH(acquiredLevel <= level, "Potential deadlock detected: Locking strategy violation");
	}

	_acquiredLockLevels.push_back(level);
}

void DeadlockDetector::PopLock(uint8 level)
{
	ASSERT_CRASH(!_acquiredLockLevels.empty(), "PopLock called on empty lock stack");
	ASSERT_CRASH(_acquiredLockLevels.back() == level, "PopLock called with mismatched lock level");
	_acquiredLockLevels.pop_back();
}
