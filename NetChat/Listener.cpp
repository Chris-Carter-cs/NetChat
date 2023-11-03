#include "Listener.h"

int Listener::listener_main() {
	runLoop = 1;
	initSocket();
	startListening();

	return 0;
}


/// <summary>
/// Initilize the socket for this componenet.
/// </summary>
int Listener::initSocket() {
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0) {
		printf("Listening socket failed with error %d\n", result);
		return 1;
	}

	listenSocket = INVALID_SOCKET;
	listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (listenSocket == INVALID_SOCKET) {
		printf("Listening socket failed with error %d\n", WSAGetLastError());
		return 1;
	}

	/*if (listenPort == 0)*/ listenPort = 5850;

	ZeroMemory(&listenAddr, sizeof(listenAddr));
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(listenPort);
	listenAddr.sin_addr.s_addr = INADDR_ANY;
	//inet_pton(AF_INET, "127.0.0.1", &listenAddr.sin_addr.s_addr);

	if (bind(listenSocket, (const struct sockaddr*)&listenAddr, sizeof(listenAddr)) < 0) {
		printf("Bind failed with error %d\n", WSAGetLastError());
		return 1;
	}

	printf("UDP Listener set up. Ready for call to recv\n");

	return 0;
}

/// <summary>
/// Begin listening for a message from another user. This function is blocking, as it calls recv(...)
/// </summary>
int Listener::startListening() {
	printf("Listening...\n");
	//Clear buffer.
	ZeroMemory(buffer, BUFFER_SIZE);
	int result = 0;
	while (true)
	{
		//printf("Listen Loop\n");
		result = recv(listenSocket, buffer, BUFFER_SIZE, 0);
		if (result < 0) {
			printf("Error in startListening function of type %d\n", WSAGetLastError());
			runLoop = 0;
			return 1;
		}
		if (result > 0) {
			//Process data...
			printf("\tLength %d Datagram received: %s\n", result, buffer);
		}
	}

	printf("This shouldn't be printed\n");
	return 0;
}

Listener::~Listener() {
	closesocket(listenSocket);
}