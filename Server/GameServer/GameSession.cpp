#include "pch.h"
#include "GameSession.h"
#include <iostream>
#include "GameSessionManager.h"

GameSession::~GameSession()
{
	std::cout << "~GameSession" << std::endl;
}

void GameSession::OnConnected()
{
	GameSessionManager::GetInstance().AddSession(std::static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GameSessionManager::GetInstance().RemoveSession(std::static_pointer_cast<GameSession>(shared_from_this()));
}

int32 GameSession::OnRecv(const RecvBuffer& buffer)
{
	int32 length = buffer.GetDataSize();
	std::cout << "Received: " << length << std::endl;

	static BYTE* dataToRead[10000];
	if (buffer.CanReadContiguously(length))
	{
		::memcpy(dataToRead, buffer.GetReadPtr(), length);
	}
	else
	{
		int32 contiguousSize = buffer.GetContiguousDataSize();
		::memcpy(dataToRead, buffer.GetReadPtr(), contiguousSize);
		::memcpy(dataToRead + contiguousSize, buffer.GetBufferPtr(), length - contiguousSize);
	}

	SendBufferPtr sendBuffer = MakeIntrusive<SendBuffer>(4096);
	sendBuffer->Write(dataToRead, length);

	// Broadcast the received data to all sessions
	GameSessionManager::GetInstance().Broadcast(sendBuffer);

	return length;
}

void GameSession::OnSend(int32 numOfBytes)
{
	std::cout << "Sent: " << numOfBytes << std::endl;
}
