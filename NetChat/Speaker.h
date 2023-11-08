#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include "STUN_IF.h"
#include <queue>

class Sender
{
	static const int MESSAGE_LENGTH = 256;


	static const int DEFAULT_PORT = 5851;
	static const int BUFFER_SIZE = 1024;

	struct sockaddr_in senderAddr;
	SOCKET senderSocket;

	//struct sockaddr_in dstAddr;
	//SOCKET dstSocket;

	WSAData data;
	short speakPort;

public:

	struct QueuedMessage {
		int status;
		sockaddr_in dest;
		char message[MESSAGE_LENGTH];
	};


	char buffer[2048];

	int init();

	void QuerySTAB(sockaddr_in _STUNAddress, UINT32* _outIP, UINT16* _outPort);

	int sendMessage();

	void EnqueueMessage(const char* _IPStr, UINT16 _port, const char* contents);

	~Sender();

	std::queue<QueuedMessage> messages;
};

