#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#ifdef _DEBUG
#define ENABLE_DEADLOCK_DETECTION
#endif // _DEBUG


#include <vector>
#include <thread>
#include <Windows.h>

#include "Types.h"

#include "CoreMacro.h"
#include "Lock.h"
