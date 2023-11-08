#pragma once


#include <iostream>
#include <thread>
#include <csignal>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Speaker.h"
#include "Listener.h"
#include "STUN_IF.h"
#include <string>
#include <format>

#pragma comment(lib, "Ws2_32.lib")

union IPK {
	UINT32 IPW;
	UINT8 IPC[4];
};

struct sessionkey {
	IPK IP;
	UINT16 sendPort;
	UINT16 listenPort;
};

bool debug;

char STUNServerName[256];
char STUNServerIP[256];
UINT16 STUNServerPort;

/// <summary>
/// Set the name of the STUN server to be queried.
/// </summary>
void SetServerName(const char* _name) { 
	if (debug) printf("Setting server name to %s.\n", _name);
	strcpy_s(STUNServerName, _name); 
}

/// <summary>
/// Set the port of the STUN server to be queried.
/// </summary>
void SetServerPort(UINT16 _port) { STUNServerPort = _port; }


std::thread* thread_listener;
std::thread* thread_sender;

Sender* sender;
Listener* listener;

WSAData wsaData;

/// <summary>
/// Read the local manifest file, stored at './config'. If no file exists, use the default.
/// </summary>
void ReadLocalManifest();

/// <summary>
/// Thread, configure and bind the listening and sending sockets.
/// </summary>
int ConfigureSockets();

/// <summary>
/// Send a query to the given STAB server to get this machine's public IP through the NAT. Returns the IP and ports packed into a single 64b number.
/// </summary>
void QuerySTAB();

/// <summary>
/// Send a simple message to the key's destination IP and port.
/// </summary>
void Touch(char* _IPString);

/// <summary>
/// Try to start a session using another users session key to connect to that user.
/// </summary>
int StartSession(INT64 _sessionKey);

/// <summary>
/// Send a message over an already existing connection.
/// </summary>
int SendMessageTo(Sender::QueuedMessage _msg);

/// <summary>
/// Used to notify the main thread of an incomming message.
/// </summary>=
std::string RecMessage(std::string _message);

/// <summary>
/// Used to handle SIGINT to wrap up threads and sockets.
/// </summary>
void signal_handler(int sig);

/// <summary>
/// Wrapper for listener.init() used to start thread.
/// </summary>
void _listenerEntry() { listener->init(); }

/// <summary>
/// Wrapper for sender.init() used to start thread.
/// </summary>
void _senderEntry() { sender->init(); }