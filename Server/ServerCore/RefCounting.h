#pragma once

class RefCounted
{
public:
	int32 AddRef()
	{
		return _refCount.fetch_add(1) + 1;
	}

	int32 ReleaseRef()
	{
		return _refCount.fetch_sub(1) - 1;
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

	using DeleterType = void(*)(void*,T*);

	static void DefaultDeleter(void*, T* ptr)
	{
		delete ptr;
	}
public:
	IntrusivePtr() = default;
	IntrusivePtr(T* ptr) : _ptr(nullptr), _deleter(DefaultDeleter) { Set(ptr); }
	IntrusivePtr(T* ptr, DeleterType deleter, void* allocator) : _ptr(nullptr), _deleter(std::move(deleter)), _allocator(allocator) { Set(ptr); }

	IntrusivePtr(const IntrusivePtr& other) : _ptr(nullptr), _deleter(other._deleter), _allocator(other._allocator) { Set(other._ptr); }
	IntrusivePtr(IntrusivePtr&& other) noexcept : _ptr(other._ptr), _deleter(std::move(other._deleter)), _allocator(other._allocator) { other._ptr = nullptr; }

	template<typename U>
	IntrusivePtr(const IntrusivePtr<U>& other) : _ptr(nullptr), _deleter(other._deleter), _allocator(other._allocator) { Set(static_cast<T*>(other._ptr)); }

	~IntrusivePtr() { Reset(); }

public:
	IntrusivePtr& operator=(const IntrusivePtr& other)
	{
		if (*this != other)
		{
			Reset();
			_deleter = other._deleter;
			_allocator = other._allocator;
			Set(other._ptr);
		}
		return *this;
	}

	IntrusivePtr& operator=(IntrusivePtr&& other) noexcept
	{
		Reset();
		_ptr = other._ptr;
		_deleter = std::move(other._deleter);
		_allocator = other._allocator;
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
		if (_ptr == nullptr)
		{
			return;
		}

		if (_ptr->ReleaseRef() == 0)
		{
			_deleter(_allocator, _ptr);
		}
		_ptr = nullptr;
	}

private:
	T* _ptr = nullptr;
	DeleterType _deleter = nullptr;
	void* _allocator = nullptr;
};
