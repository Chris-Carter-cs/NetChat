#include "STUN_IF.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

char STUNServer[256];
int STUNPort;

std::string NameToIP(const char* hostname) {
	char ip[100];
	struct addrinfo hints, * servinfo, * p;
	struct sockaddr_in* h;
	int rv;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, "http", &hints, &servinfo)) != 0) {
		printf("No addr info found: %d.\n", WSAGetLastError());
		return "";
	}
	for (p = servinfo; p != NULL; p = p->ai_next) {
		h = (struct sockaddr_in*)p->ai_addr;
		//strcpy_s(ip, inet_ntoa(h->sin_addr));
		inet_ntop(AF_INET, &h->sin_addr, ip, sizeof(ip));
	}
	freeaddrinfo(servinfo);


	return std::string(ip);
}

/// <summary>
/// Set the name of the STUN server to be queried.
/// </summary>
void SetServerName(const char* _name) {
	ZeroMemory(STUNServer, 256);
	strcpy_s(STUNServer, NameToIP(_name).c_str());
}

/// <summary>
/// Set the port of the STUN server to be queried.
/// </summary>
void SetServerPort(int _port) {
	STUNPort = _port;
}

void TestConnection() {
	struct sockaddr_in server_addr;
	struct sockaddr_in local_addr;
	SOCKET ssocket;

	BindingRequest request;
	BindingResponse response;


	//Local Socket setup
	ssocket = INVALID_SOCKET;
	ssocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ssocket == INVALID_SOCKET) {
		printf("STUN socket failed with error %d\n", WSAGetLastError());
		return;
	}

	//Set up local address
	ZeroMemory(&local_addr, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(5851);

	//Bind local server to socket.
	if (bind(ssocket, (SOCKADDR*)&local_addr , sizeof(local_addr))) {
		printf("STUN bind failed with error %d\n", WSAGetLastError());
		return;
	}

	//Set up STUN server's address
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(STUNPort);
	inet_pton(AF_INET, STUNServer, &server_addr.sin_addr.s_addr);

	//Set up the values for the call to `select()`
	fd_set set;
	set.fd_count = 1;
	set.fd_array[0] = ssocket;

	timeval timer;
	timer.tv_sec = 5;
	timer.tv_usec = 0;

	int result = 0;
	int attempts = 0;

	//while (attempts < 5) {

		//Send the request to the STUN server.
		printf("Sending request to STUN server at '%s:%d'.\n", STUNServer, STUNPort);
		if (sendto(ssocket, (char*)&request, sizeof(request), 0, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			printf("Error while sending packet to STUN server: %d\n", WSAGetLastError());
			return;
		}

		/*result = select(0, &set, NULL, &set, &timer);

		if (result == SOCKET_ERROR) {
			printf("Socket error on select: %d\n", WSAGetLastError());
			return;
		}

		attempts++;
	}

	//See if the message was ever received.
	if (attempts == 5) {
		printf("No message received after 5 attempts. Aborting.\n");
		return;
	}*/

	//Wait for a response for the server.
	int res;
	ZeroMemory(&response, sizeof(response));
	res = recvfrom(ssocket, (char*)&response, sizeof(response), 0, NULL, 0);
	if (res < 0) {
		printf("Error while receiving packet to STUN server: %d\n", WSAGetLastError());
		return;
	}

	printf("Response received:\n\tType:%x\tLength: %u\n", response.Type, response.Length);

	int attributeOffset = 0;
	UINT16 type, length;
	do {
		type = htons(*(UINT16*)(response.Attributes + attributeOffset));
		length = htons(*(UINT16*)(response.Attributes + attributeOffset + 2));

		printf("\tAttribute Type: %x, \tLength: %u\n", type, length);

		if (type == MAPPED_ADDR_ATTRIBUTE) {
			printf("Mapped address attribute found\n\tIP: ");
			printf("%u.", *(UINT8*)(response.Attributes + attributeOffset + 8));
			printf("%u.", *(UINT8*)(response.Attributes + attributeOffset + 9));
			printf("%u.", *(UINT8*)(response.Attributes + attributeOffset + 10));
			printf("%u:", *(UINT8*)(response.Attributes + attributeOffset + 11));

			printf("%u\n", *(UINT16*)(response.Attributes + attributeOffset + 6));

			closesocket(ssocket);
			return;
		}
		attributeOffset += length + 4;
	} while (type != 0 && attributeOffset < 1028);


	
	/*printf("\tType:%x\tLength: %d\n", ntohs(response.AttributeType), ntohs(response.AttributeLength));
	printf("\tFamily:%x\tPort: %d\n", response.Family, ntohs(response.Port));
	
	char* addrptr = (char*)&response.Address;
	printf("\tAddress: %u.%u.%u.%u\n", addrptr[0], addrptr[1], addrptr[2], addrptr[3]);*/
	printf("Attribute not found.\n");
	closesocket(ssocket);
	return;
}