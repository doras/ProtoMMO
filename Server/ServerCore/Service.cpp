#include "pch.h"
#include "Service.h"
#include "IocpCore.h"
#include "Listener.h"
#include "Session.h"

SessionPtr Service::CreateSession()
{
	if (_sessionFactory == nullptr)
		return nullptr;

	SessionPtr session = _sessionFactory();
	if (_iocpCore->Associate(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(SessionPtr session)
{
	WRITE_LOCK;
	_sessions.insert(session);
	_sessionCount++;
}

void Service::RemoveSession(SessionPtr session)
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) == 1, "Session not found in service");
	_sessionCount--;
}

bool ClientService::Start()
{
	// TODO
	return false;
}

bool ServerService::Start()
{
	if (_sessionFactory == nullptr)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerServicePtr self = std::static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->Start(self) == false)
		return false;

	return true;
}
