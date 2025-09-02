#include "pch.h"

#include "Core.h"
#include "SocketUtils.h"

#include <iostream>

#include "Listener.h"
#include "IocpCore.h"

int main()
{
	ListenerPtr listener = MakeShared<Listener>();
	listener->Start({ L"127.0.0.1", 7777 });

	for (int32 i = 0; i < 4; ++i)
	{
		GThreadManager->Launch([]()
		{
			while (true)
			{
				GIocpCore.PollAndDispatch();
			}
		});
	}

	GThreadManager->Join();
}
