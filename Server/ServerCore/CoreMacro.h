#pragma once

/*---------
	LOCK
---------*/

#define USE_LOCK(level)		Lock _lock = static_cast<uint8>(level);
#define READ_LOCK			ReadLockGuard readLockGuard(_lock);
#define WRITE_LOCK			WriteLockGuard writeLockGuard(_lock);

#ifdef _DEBUG
#define ENABLE_DEADLOCK_DETECTION
#endif // _DEBUG

/*-----------
	MEMORY
------------*/
#ifdef _DEBUG
#define DEFAULT_ALLOCATOR normalAllocator
//#define DEFAULT_ALLOCATOR stompAllocator
#else
#define DEFAULT_ALLOCATOR normalAllocator
#endif // _DEBUG

/*-----------
	CRASH
-----------*/

#define CRASH(cause)	\
do						\
{						\
	((void (*)())0)();	\
}						\
while (false)

#define ASSERT_CRASH(expr, cause)		\
do										\
{										\
	if (!(expr))						\
	{									\
		CRASH(cause);					\
	}									\
}										\
while (false)
