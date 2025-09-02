#pragma once

#include <atomic>

using int8		= signed	char;
using int16		= signed	short;
using int32		= signed	int;
using int64		= signed	long long;
using uint8		= unsigned	char;
using uint16	= unsigned	short;
using uint32	= unsigned	int;
using uint64	= unsigned	long long;

template<typename T>
using Atomic = std::atomic<T>;

using IIoHandlerPtr = std::shared_ptr<class IIoHandler>;
using ListenerPtr = std::shared_ptr<class Listener>;
using SessionPtr = std::shared_ptr<class Session>;
