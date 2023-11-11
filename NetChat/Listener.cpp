#include "Listener.h"

int Listener::init() {
	initSocket();
	callRecv = 0;
	runLoop = 1;
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

	listenPort = 5850;

	ZeroMemory(&listenAddr, sizeof(listenAddr));
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(listenPort);
	listenAddr.sin_addr.s_addr = INADDR_ANY;

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
	while (runLoop)
	{
		if (callRecv) {
			result = recv(listenSocket, buffer, BUFFER_SIZE, 0);
			if (result < 0) {
				printf("Error in startListening function of type %d\n", WSAGetLastError());
				return 1;
			}
			if (result > 0) {
				//Process data...
				printf("\tLength %d Datagram received:\n\t%s\n", result, buffer);
			}
		}
	}
	return 0;
}

void Listener::QuerySTAB(sockaddr_in _STUNAddress, UINT32* _outIP, UINT16* _outPort) {
	//Declare values for the request and response.
	BindingRequest request;
	BindingResponse response;

	//Send the request
	printf("Sending STUN request for Sender.\n");
	sendto(listenSocket, (char*)&request, sizeof(request), 0, (sockaddr*)&_STUNAddress, sizeof(_STUNAddress));

	//Set up the values for the listening loop
	INT64 timeout = 5000l;

	int attempt = 0;
	int result;
	setsockopt(listenSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	do {
		printf("Waiting for STUN response for Listener (%d/%d)... ", attempt + 1, 8);
		result = recv(listenSocket, (char*)&response, sizeof(response), 0);

		if (result == SOCKET_ERROR) {
			result = WSAGetLastError();
			if (result == WSAETIMEDOUT) {
				//Timed out
				printf("Time out.\n");
				attempt++;
			}
			else {
				printf("Error in Listener's STUN request: %d\n", result);
				exit(1);
			}
		}

		printf("STUN response for Listener\n");

		printf("Response received:\n\tType:%x\tLength: %u\n", response.Type, response.Length);

		int attributeOffset = 0;
		UINT16 type, length;
		do {
			type = htons(*(UINT16*)(response.Attributes + attributeOffset));
			length = htons(*(UINT16*)(response.Attributes + attributeOffset + 2));

			printf("\tAttribute Type: %x, \tLength: %u\n", type, length);

			if (type == MAPPED_ADDR_ATTRIBUTE) {
				
				*_outIP = (*(UINT32*)(response.Attributes + attributeOffset + 8));
				*_outPort = ((*(UINT16*)(response.Attributes + attributeOffset + 6)));	//Sender port is from bit 16-32
				return;
			}
			attributeOffset += length + 4;
		} while (type != 0 && attributeOffset < 1028);

	} while (attempt < 8);

	printf("STUN response for Listener timed out %d times. Aborting...\n", attempt);
	exit(1);
}

Listener::~Listener() {
	closesocket(listenSocket);
}