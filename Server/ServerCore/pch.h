#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <thread>
#include <Windows.h>
#include <memory>

#include "Types.h"

#include "CoreMacro.h"
#include "ManagerInitializer.h"
#include "Lock.h"
#include "Container.h"
#include "RefCounting.h"
