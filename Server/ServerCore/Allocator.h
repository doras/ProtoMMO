#pragma once

class IAllocator
{
public:
	virtual void* Allocate(size_t size) = 0;
	virtual void  Deallocate(void* ptr) = 0;

	virtual ~IAllocator() = default;
};

class NormalAllocator : public IAllocator
{
public:
	virtual void* Allocate(size_t size) override;
	virtual void  Deallocate(void* ptr) override;
};
