#pragma once

#define CRASH(cause)	\
do						\
{						\
	((void (*)())0)();	\
}						\
while (false)

#define ASSERT_CRASH(expr)		\
do								\
{								\
	if (!(expr))				\
	{							\
		CRASH("ASSERT_CRASH");	\
	}							\
}								\
while (false)
