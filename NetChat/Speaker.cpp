#include "Speaker.h"

int Speaker::speaker_main() { 
	initSocket();
	Sleep(2);
	sendMessage();
	return 0;
}



int Speaker::initSocket() { 
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0) {
		printf("Speaking socket failed with error %d\n", result);
		return 1;
	}

	speakerSocket = INVALID_SOCKET;
	speakerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (speakerSocket == INVALID_SOCKET) {
		printf("Speaker socket failed with error %d\n", WSAGetLastError());
		return 1;
	}

	ZeroMemory(&speakerAddr, sizeof(speakerAddr));
	speakerAddr.sin_family = AF_INET;
	speakerAddr.sin_port = htons(speakPort);
	inet_pton(AF_INET, "127.0.0.1", &speakerAddr.sin_addr.s_addr);

	if (bind(speakerSocket, (SOCKADDR*)&speakerAddr, sizeof(speakerAddr))) {
		printf("Speaker bind failed with error %d\n", WSAGetLastError());
		return 1;
	}

	printf("UDP Speaker set up. Ready for call to sendto.\n");
	return 0;
}
int Speaker::sendMessage() {
	printf("Sending message...\n");
	ZeroMemory(&dstAddr, sizeof(dstAddr));
	dstAddr.sin_family = AF_INET;
	dstAddr.sin_port = htons(5850);
	inet_pton(AF_INET, "127.0.0.1", &dstAddr.sin_addr.s_addr);

	ZeroMemory(buffer, BUFFER_SIZE);
	strcpy_s(buffer, "Cross-port message!");

	int result;
	result = sendto(speakerSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&dstAddr, BUFFER_SIZE);
	if (result < 0) {
		printf("Error in send of type %d\n", WSAGetLastError());
	}
	printf("Message sent!\n");

	return 0;
}

Speaker::~Speaker() {
	closesocket(speakerSocket);
}
