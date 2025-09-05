#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

void GameSessionManager::AddSession(GameSessionPtr session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void GameSessionManager::RemoveSession(GameSessionPtr session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void GameSessionManager::Broadcast(const SendBufferPtr& sendBuffer)
{
	READ_LOCK;
	for (const GameSessionPtr& session : _sessions)
	{
		session->Send(sendBuffer);
	}
}
