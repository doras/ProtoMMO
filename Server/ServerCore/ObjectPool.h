#pragma once

#include "Types.h"
#include "MemoryPool.h"

template<typename T>
class ObjectPool
{
public:
	template<typename... Args>
	static T* New(Args&&... args)
	{
		T* ptr = static_cast<T*>(MemoryHeader::AttachHeader(sPool.Pop(), sBlockSize));
		return new (ptr) T(std::forward<Args>(args)...);
	}

	static void Delete(T* obj)
	{
		if (!obj)
		{
			return;
		}
		obj->~T();
		sPool.Push(MemoryHeader::DetachHeader(obj));
	}

private:
	static int32 sBlockSize;
	static MemoryPool sPool;
};

template<typename T>
int32 ObjectPool<T>::sBlockSize = sizeof(T) + sizeof(MemoryHeader);

template<typename T>
MemoryPool ObjectPool<T>::sPool(ObjectPool<T>::sBlockSize);
