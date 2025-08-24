#pragma once

/*---------
	LOCK
---------*/

#define USE_LOCK(level)		Lock _lock = static_cast<uint8>(level);
#define READ_LOCK			ReadLockGuard readLockGuard(_lock);
#define WRITE_LOCK			WriteLockGuard writeLockGuard(_lock);

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
