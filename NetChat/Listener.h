#pragma once
//#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>



class Listener
{
	static const int DEFAULT_PORT = 5850;
	static const int BUFFER_SIZE = 1024;

	struct sockaddr_in listenAddr;
	SOCKET listenSocket;
	WSAData data;
	short listenPort;

	char buffer[2048];

public:
	int runLoop;

	int listener_main();
	int initSocket();
	int startListening();

	~Listener();
};

