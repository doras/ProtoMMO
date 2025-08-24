#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadId = 0;

#ifdef ENABLE_DEADLOCK_DETECTION
#include "DeadlockDetector.h"
thread_local DeadlockDetector LDeadlockDetector;
#endif // ENABLE_DEADLOCK_DETECTION
