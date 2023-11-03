#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

class Speaker
{
	static const int DEFAULT_PORT = 5851;
	static const int BUFFER_SIZE = 1024;

	struct sockaddr_in speakerAddr;
	SOCKET speakerSocket;

	struct sockaddr_in dstAddr;
	//SOCKET dstSocket;

	WSAData data;
	short speakPort;

public:

	char buffer[2048];

	int speaker_main();
	int initSocket();
	int sendMessage();

	~Speaker();
};

