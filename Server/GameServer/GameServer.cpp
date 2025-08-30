#include "pch.h"

#include "Core.h"
#include "SocketUtils.h"

#include <iostream>

int main()
{
	SOCKET socket = SocketUtils::CreateSocket();

	SocketUtils::BindAnyAddr(socket, 7777);

	SocketUtils::Listen(socket);

	SOCKET clientSocket = ::accept(socket, nullptr, nullptr);

	std::cout << "Client connected." << std::endl;

	while (true)
	{

	}

	GThreadManager->Join();
}
