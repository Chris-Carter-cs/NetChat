#include "Speaker.h"

int Sender::init() {
	int result;

	//Create socket
	senderSocket = INVALID_SOCKET;
	senderSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (senderSocket == INVALID_SOCKET) {
		printf("Error in socket creation in Sender::init(): %d\n", WSAGetLastError());
		exit(1);
	}

	//Configure local address
	ZeroMemory(&senderAddr, sizeof(senderAddr));
	senderAddr.sin_family = AF_INET;
	senderAddr.sin_port = htons(speakPort);
	
	//Bind address to socket
	result = bind(senderSocket, (sockaddr*)&senderAddr, sizeof(senderAddr));
	if (result < 0) {
		printf("Error in socket binding in Sender::init(): %d\n", WSAGetLastError());
		exit(1);
	}

	return 0;
}

void Sender::QuerySTAB(sockaddr_in _STUNAddress, UINT32* _outIP, UINT16* _outPort) {
	//Declare values for the request and response.
	BindingRequest request;
	BindingResponse response;

	//Send the request
	printf("Sending STUN request for Sender.\n");
	sendto(senderSocket, (char*)&request, sizeof(request), 0, (sockaddr*)&_STUNAddress, sizeof(_STUNAddress));

	//Set up the values for the call to `select()`
	INT64 timeout = 5000l;

	int attempt = 0;
	int result;
	setsockopt(senderSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	do {
		printf("Waiting for STUN response for Sender (%d/%d)... ", attempt+1, 8);
		result = recv(senderSocket, (char*)&response, sizeof(response), 0);

		if (result == SOCKET_ERROR) {
			result = WSAGetLastError();
			if (result == WSAETIMEDOUT) {
				//Timed out
				printf("Time out.\n");
				attempt++;
			}
			else {
				printf("Error in Sender's STUN request: %d\n", result);
				exit(1);
			}
		}
		
		printf("STUN response for Sender\n");

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

	printf("STUN response for sender timed out %d times. Aborting...\n", attempt);
	exit(1);
}



int Sender::sendMessage() {
	printf("Sending message...\n");

	QueuedMessage msg = messages.front();

	int result;
	result = sendto(senderSocket, msg.message, MESSAGE_LENGTH, 0, (struct sockaddr*)&msg.dest, MESSAGE_LENGTH);
	if (result < 0) {
		printf("Error in send of type %d\n", WSAGetLastError());
		return 1;
	}
	printf("Message sent!\n");

	return 0;
}

void Sender::EnqueueMessage(const char* _IPStr, UINT16 _port, const char* contents) {
	QueuedMessage msg;
	ZeroMemory(&msg, sizeof(msg));
	//msg.dest.sin_addr = _ip;
	msg.dest.sin_port = _port;
	msg.dest.sin_family = AF_INET;
	inet_pton(AF_INET, _IPStr, &msg.dest.sin_addr.s_addr);

	//STUNAddress.sin_port = htons(STUNServerPort);
	//inet_pton(AF_INET, STUNServerIP, &STUNAddress.sin_addr.s_addr);

	char c;
	int i = 0;
	do {
		c = contents[i];
		msg.message[i] = c;
		i++;
	} while (c != 0 && i < MESSAGE_LENGTH - 1);

	messages.push(msg);
}


Sender::~Sender() {
	closesocket(senderSocket);
}
