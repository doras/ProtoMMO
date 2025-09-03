#include "pch.h"
#include <iostream>

#include "Core.h"
#include "Service.h"
#include "Session.h"
#include "IocpCore.h"


#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

BYTE data[1000];


class GameSession : public Session
{
public:
	~GameSession()
	{
		std::cout << "~GameSession" << std::endl;
	}

	virtual int32 OnRecv(const RecvBuffer& buffer) override
	{
		int32 length = buffer.GetDataSize();
		std::cout << "Received: " << length << std::endl;

		const BYTE* dataToRead;
		if (buffer.CanReadContiguously(length))
		{
			dataToRead = buffer.GetReadPtr();
		}
		else
		{
			int32 contiguousSize = buffer.GetContiguousDataSize();
			::memcpy(data, buffer.GetReadPtr(), contiguousSize);
			::memcpy(data + contiguousSize, buffer.GetBufferPtr(), length - contiguousSize);
			dataToRead = reinterpret_cast<const BYTE*>(data);
		}

		// Echo back
		Send(dataToRead, length);

		return length;
	}

	virtual void OnSend(int32 numOfBytes) override
	{
		std::cout << "Sent: " << numOfBytes << std::endl;
	}
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
