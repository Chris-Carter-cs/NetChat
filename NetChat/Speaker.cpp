#include "Speaker.h"

int Speaker::speaker_main() { return 0; }



int Speaker::initSocket() { 
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0) {
		printf("Speaking socket failed with error %d\n", result);
		return 1;
	}

	SOCKET speakerSocket = INVALID_SOCKET;
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

	printf("UDP Speaker set up. Ready for call to recv.");
	return 0;
}
int Speaker::sendMessage() {
	ZeroMemory(&dstAddr, sizeof(dstAddr));
	dstAddr.sin_port = 5850;
	inet_pton(AF_INET, "127.0.0.1", &dstAddr.sin_addr.s_addr);

	int result;
	result = sendto(speakerSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&dstAddr, BUFFER_SIZE);
	
	return 0;

}
