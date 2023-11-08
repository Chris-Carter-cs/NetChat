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
    
    char line[256];
    ZeroMemory(&line, sizeof(line));

    printf("Type 'help' to see commands; q to quit.\n");
    //Basic Shell Loop
    while (1) {
        if (sender->messages.size() > 0) {
            printf("Message queue is greater than 0.\n");
            sender->sendMessage();
        }

        printf("NetChat> ");
        std::cin >> line;
        printf("\n");
        if (strcmp(line, "q") == 0) {
            listener->runLoop = 0;
            thread_listener->join();
            //thread_sender->join();

            delete listener;
            delete sender;

            delete thread_listener;
            //delete thread_sender;
            exit(0);
        }
        else if (strcmp(line, "help") == 0) {
            printf("Commands:\n\tq\t-\tClose program.\n");
            printf("\thelp\t-\tPrint this prompt.\n");
            printf("\tkey\t-\tSend a STUN request to get this programs public keys.\n");
            printf("\ttch\t-\t.Send a basic message to a given public key to test connection.\n");
        }
        else if (strcmp(line, "key") == 0) {
            QuerySTAB();
            
        }
        else if (strcmp(line, "tch") == 0) {
            printf("Input <IP:Port> for other user: ");
            std::cin >> line;
            printf("\n");
            Touch(line);
            //printf("Touch1 has been sent.\n");
            //Sleep(3500);
            //Touch(line);
            //printf("Touch2 has been sent.\n");
        }
    }

    return 0;
}

int ConfigureSockets() {
    //Initilize sender and listener objects.
    sender = new Sender();
    listener = new Listener();

    if (debug) printf("Starting listener and sender threads...\n");

    //Initilize sender and listener threads.
    thread_listener = new std::thread(&_listenerEntry);
    //hread_sender = new std::thread(&_senderEntry);
    _senderEntry();

    return 0;
}

void QuerySTAB() {
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

    //printf("Sender IP:\n\t%x\n", senderIP);
    //printf("Listen IP:\n\t%x\n", listenerIP);


    //Compare the results.
    if (senderIP != listenerIP) {
        printf("Mismatch of IP's for sender and listener.\n");
        exit(1);
    }

    sessionkey key;
    key.IP.IPW = senderIP;
    key.listenPort = listenerPort;
    key.sendPort = senderPort;

    
    std::string output = std::to_string(key.IP.IPC[0]);
    output += ".";
    output += std::to_string(key.IP.IPC[1]);
    output += ".";
    output += std::to_string(key.IP.IPC[2]);
    output += ".";
    output += std::to_string(key.IP.IPC[3]);
    
    printf("Receiver:\n");
    printf("%d.%d.%d.%d:%d\n", key.IP.IPC[0], key.IP.IPC[1], key.IP.IPC[2], key.IP.IPC[3], key.listenPort);
    printf("Sender:\n");
    printf("%d.%d.%d.%d:%d\n", key.IP.IPC[0], key.IP.IPC[1], key.IP.IPC[2], key.IP.IPC[3], key.sendPort);
}

void Touch(char* _IPString) {
    std::string str(_IPString);
    int s = str.find(":");
    std::string pstr = str.substr(s + 1, str.size() - s);
    //printf("pstr: %s\n", pstr.c_str());
    UINT16 port = std::stoul(pstr);
    
    
    printf("Debugged call to touch with the following values:\n");
    printf("\tIP: %s\n\tPort: %d\n", str.substr(0, s).c_str(), port);

    sender->EnqueueMessage(str.substr(0, s).c_str(), port, "Test Message.");
}

