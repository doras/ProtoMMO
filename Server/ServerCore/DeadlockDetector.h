#pragma once

class DeadlockDetector
{
public:
	DeadlockDetector() {}
	~DeadlockDetector() {}

	void PushLock(uint8 level);
	void PopLock(uint8 level);

private:
	std::vector<uint8> _acquiredLockLevels;
};

