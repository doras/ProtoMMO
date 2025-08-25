#pragma once

class RefCounted
{
public:
	int32 AddRef()
	{
		return ++_refCount;
	}

	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}

protected:
	RefCounted() : _refCount(0) {}
	virtual ~RefCounted() {}

private:
	Atomic<int32> _refCount;
};

template<typename T>
class IntrusivePtr
{
	static_assert(std::is_base_of<RefCounted, T>::value, "T must be derived from RefCounted");
public:
	IntrusivePtr() {}
	IntrusivePtr(T* ptr) { Set(ptr); }

	IntrusivePtr(const IntrusivePtr& other) { Set(other._ptr); }
	IntrusivePtr(IntrusivePtr&& other) noexcept : _ptr(other._ptr) { other._ptr = nullptr; }

	template<typename U>
	IntrusivePtr(const IntrusivePtr<U>& other) { Set(static_cast<T*>(other._ptr)); }

	~IntrusivePtr() { Reset(); }

public:
	IntrusivePtr& operator=(const IntrusivePtr& other)
	{
		if (*this != other)
		{
			Reset();
			Set(other._ptr);
		}
		return *this;
	}

	IntrusivePtr& operator=(IntrusivePtr&& other) noexcept
	{
		Reset();
		_ptr = other._ptr;
		other._ptr = nullptr;
		return *this;
	}

	bool		operator==(const IntrusivePtr& other) const { return _ptr == other._ptr; }
	bool		operator==(const T* ptr) const { return _ptr == ptr; }
	bool		operator!=(const IntrusivePtr& other) const { return _ptr != other._ptr; }
	bool		operator!=(const T* ptr) const { return _ptr != ptr; }
	bool		operator<(const IntrusivePtr& other) const { return _ptr < other._ptr; }
	bool		operator<(const T* ptr) const { return _ptr < ptr; }

	T&			operator*() { return *_ptr; }
	const T&	operator*() const { return *_ptr; }
	T*			operator->() { return _ptr; }
	const T*	operator->() const { return _ptr; }

	operator bool() const { return _ptr != nullptr; }
	T* Get() const { return _ptr; }

private:
	void Set(T* ptr)
	{
		_ptr = ptr;
		if (_ptr)
		{
			_ptr->AddRef();
		}
	}

	void Reset()
	{
		if (_ptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};

template<typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args&&... args)
{
	return IntrusivePtr<T>(new T(std::forward<Args>(args)...));
}
