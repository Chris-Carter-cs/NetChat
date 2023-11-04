#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <cstdlib>
#include <string>

#pragma pack(push, 1)

static const UINT16 RESPONSE = 0x0101;
static const UINT16 MAPPED_ADDR_ATTRIBUTE = 0x0001;

/// <summary>
/// Binding request. Fills out type and length to the default and creates a random transaction ID.
/// </summary>
struct BindingRequest {
	INT16 Type;
	INT16 Length;
	char TransactionID[16];	//128b Transaction ID.

	BindingRequest() {
		Type = htons(0x0001);
		Length = htons(0x0000);
		for (int i = 0; i < 16; i++) {
			TransactionID[i] = rand() % 256;
		}
	}
};

struct BindingResponse {
	//Header
	INT16 Type;
	INT16 Length;
	INT8 TransactionID[16];	//128b Transaction ID.
	//Attributes
	INT8 Attributes[1024];
};

/// <summary>
/// Set the name of the STUN server to be queried.
/// </summary>
void SetServerName(const char* _name);

/// <summary>
/// Set the port of the STUN server to be queried.
/// </summary>
void SetServerPort(int _port);


void TestConnection();