#include "pch.h"
#include <iostream>

#include "Core.h"
#include "Service.h"
#include "Session.h"
#include "IocpCore.h"


class GameSession : public Session
{
};

int main()
{
	ServerServicePtr service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		32
	);

	if (service->Start() == false)
	{
		std::cout << "Failed to start service" << std::endl;
		return -1;
	}

	for (int32 i = 0; i < 4; ++i)
	{
		GThreadManager->Launch([service]()
		{
			while (true)
			{
				service->GetIocpCore()->PollAndDispatch();
			}
		});
	}

	GThreadManager->Join();
}
