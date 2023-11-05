#pragma once
//#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "STUN_IF.h"



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
	int callRecv;

	int init();

	void QuerySTAB(sockaddr_in _STUNAddress, UINT32* _outIP, UINT16* _outPort);

	int initSocket();
	int startListening();

	~Listener();
};

