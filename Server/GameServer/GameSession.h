#pragma once

#include "Session.h"

class GameSession : public Session
{
public:
	~GameSession();

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual int32 OnRecv(const RecvBuffer& buffer) override;
	virtual void OnSend(int32 numOfBytes) override;
};
