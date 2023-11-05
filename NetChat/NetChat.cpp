// NetChat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "NetChat.h"

int main()
{
    debug = 1;

    //WSA Initilization.
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSA startup failed with error %d\n", result);
        return 0;
    }

    //STUN server:   stun.l.google.com:19302
    SetServerName("stun.l.google.com");
    SetServerPort(19302);

    ConfigureSockets();
    QuerySTAB();

    return 0;
}

int ConfigureSockets() {
    //Initilize sender and listener objects.
    sender = new Sender();
    listener = new Listener();

    if (debug) printf("Starting listener and sender threads...\n");

    //Initilize sender and listener threads.
    thread_listener = new std::thread(&_listenerEntry);
    thread_sender = new std::thread(&_senderEntry);

    return 0;
}

UINT64 QuerySTAB() {
    if (debug) printf("Starting the parent's QuerySTAB() function.\n");
    //TODO:: Left off on this function last night.
        //Should query the STAB server using both the sending and receiving sockets.
        //The resulting information is used to create the session ID, with the format:
        /*
        0b              16b             32b
        +--  --  --  -- |--  --  --  --+
        |   Sender Port | Listen Port  |
        |--  --  --  -- |--  --  --  --+
        |           IP Address         |
        +--  --  --  --  --  --  --  --+
        */
    //Need to get the IP of the STUN server from the name.
    struct addrinfo hints;
    struct addrinfo* server;
    
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    //Get the information from the DNS(?)
    if (getaddrinfo(STUNServerName, "http", &hints, &server) != 0) {
        printf("Error in finding server name in NetChat::QuerySTAB(): %d\n", WSAGetLastError());
        exit(1);
    }

    struct sockaddr_in* STUNAddress_p;
    ZeroMemory(&STUNAddress_p, sizeof(STUNAddress_p));
    struct addrinfo* infoPointer;

    for (infoPointer = server; infoPointer != NULL; infoPointer = infoPointer->ai_next) {
        STUNAddress_p = (struct sockaddr_in*)infoPointer->ai_addr;
        inet_ntop(AF_INET, &STUNAddress_p->sin_addr, STUNServerIP, sizeof(STUNServerIP));
    }
    freeaddrinfo(server);


    struct sockaddr_in STUNAddress;
    //Finish setting up STUN server's address
    ZeroMemory(&STUNAddress, sizeof(STUNAddress));
    STUNAddress.sin_family = AF_INET;
    STUNAddress.sin_port = htons(STUNServerPort);
    inet_pton(AF_INET, STUNServerIP, &STUNAddress.sin_addr.s_addr);

    //Query the STAB using both sockets.
    UINT32 senderIP = 0;
    UINT16 senderPort = 0;

    UINT32 listenerIP = 0;
    UINT16 listenerPort = 0;

    sender->QuerySTAB(STUNAddress, &senderIP, &senderPort);
    listener->QuerySTAB(STUNAddress, &listenerIP, &listenerPort);
    UINT64 skey = 0l;

    printf("Sender IP:\n\t%x\n", senderIP);
    printf("Listen IP:\n\t%x\n", listenerIP);


    thread_listener->join();
    thread_sender->join();
    delete thread_listener;
    delete thread_sender;
    delete listener;
    delete sender;
    exit(0);

    //Compare the results.
    if (senderIP != listenerPort) {
        printf("Mismatch of IP's for sender and listener.\n");
        exit(1);
    }

    return 0l;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
