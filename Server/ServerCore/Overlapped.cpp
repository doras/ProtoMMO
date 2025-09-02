#include "pch.h"
#include "Overlapped.h"

OverlappedBase::OverlappedBase(IOType type) : type(type), owner()
{
	Init();
}

void OverlappedBase::Init()
{
	::memset(this, 0, sizeof(OVERLAPPED));
}
