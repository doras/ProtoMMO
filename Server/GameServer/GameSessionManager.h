#pragma once

class GameSessionManager
{
	USE_LOCK(LockLevelInternal::GameSessionManager)
public:
	static GameSessionManager& GetInstance()
	{
		static GameSessionManager instance;
		return instance;
	}

	void AddSession(GameSessionPtr session);
	void RemoveSession(GameSessionPtr session);
	void Broadcast(const SendBufferPtr& sendBuffer);

private:
	GameSessionManager() = default;

	HashSet<GameSessionPtr> _sessions;
};
