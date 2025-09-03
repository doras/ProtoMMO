#pragma once
#include <functional>
#include "NetAddress.h"

using SessionFactory = std::function<SessionPtr(void)>;

// Base class for a service as a network endpoint.
class Service : public std::enable_shared_from_this<Service>
{
public:
	enum class Type : uint8
	{
		Client,
		Server
	};

	Service() = default;
	Service(Type type, const NetAddress& address, IocpCorePtr iocpCore, SessionFactory sessionFactory, int32 maxSessionCount = 1)
		: _type(type), _address(address), _iocpCore(iocpCore), _maxSessionCount(maxSessionCount), _sessionFactory(sessionFactory) {}
	virtual ~Service() {}

	virtual bool	Start() = 0;

	// Create a new session and associate it with the IOCP
	// WITHOUT adding it to the session list
	SessionPtr		CreateSession();

	void			AddSession(SessionPtr session);
	void			RemoveSession(SessionPtr session);

public:
	IocpCorePtr			GetIocpCore() const { return _iocpCore; }
	const NetAddress&	GetAddress() const { return _address; }
	int32				GetMaxSessionCount() const { return _maxSessionCount; }
	Type				GetType() const { return _type; }

protected:
	USE_LOCK(LockLevelInternal::Service)

	Type				_type;
	NetAddress			_address = {}; // Target or Listen address
	IocpCorePtr			_iocpCore = nullptr;

	HashSet<SessionPtr> _sessions = {}; // For only connected sessions
	int32 				_sessionCount = 0;
	int32				_maxSessionCount = 0; // Connected sessions for client, pending AcceptExs for server
	SessionFactory		_sessionFactory = nullptr;
};

class ClientService : public Service
{
public:
	ClientService() = default;
	ClientService(const NetAddress& targetAddress, IocpCorePtr iocpCore, SessionFactory sessionFactory, int32 maxSessionCount = 1)
		: Service(Type::Client, targetAddress, iocpCore, sessionFactory, maxSessionCount) {}
	virtual ~ClientService() {}

	virtual bool	Start() override;
};

class ServerService : public Service
{
public:
	ServerService() = default;
	ServerService(const NetAddress& listenAddress, IocpCorePtr iocpCore, SessionFactory sessionFactory, int32 maxSessionCount = 1)
		: Service(Type::Server, listenAddress, iocpCore, sessionFactory, maxSessionCount) {}
	virtual ~ServerService() {}

	virtual bool	Start() override;

private:
	ListenerPtr		_listener = nullptr;
};
