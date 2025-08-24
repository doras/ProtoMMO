#pragma once

extern thread_local uint32 LThreadId;

#ifdef ENABLE_DEADLOCK_DETECTION
extern thread_local class DeadlockDetector LDeadlockDetector;
#endif // ENABLE_DEADLOCK_DETECTION
