// NetChat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <csignal>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Speaker.h"
#include "Listener.h"
#include "STUN_IF.h"

#pragma comment(lib, "Ws2_32.lib")

std::thread* thread_listener;
std::thread* thread_speaker;

Speaker* speaker;
Listener* listener;


void signal_handler(int sig);

void signal_handler(int sig) {
    listener->runLoop = 0;

    printf("Cleaning up WSA...\n");
    WSACleanup();
    
    printf("Signal Handler called. Joining threads...\n");
    thread_listener->join();
    thread_speaker->join();

    printf("Threads joined. Deleting thread objects...\n");

    delete listener;
    delete speaker;

    delete thread_listener;
    delete thread_speaker;

    printf("Finished. Exiting.\n");

    

    exit(0);
}

void startListener() {
    listener->listener_main();
}

void startSpeaker() {
    speaker->speaker_main();
}

int main()
{
    /*speaker = new Speaker();
    listener = new Listener();


    printf("Setting signal handler\n");
    signal(SIGINT, signal_handler);

    printf("Initilizing listener thread...\n");
    thread_listener = new std::thread(&startListener);
    
    Sleep(2000);

    printf("Initilizing speaker thread...\n");
    thread_speaker = new std::thread(&startSpeaker);*/

    WSAData data;
    int result = WSAStartup(MAKEWORD(2, 2), &data);
    if (result != 0) {
        printf("STUN socket failed with error %d\n", result);
        return 0;
    }

    //STUN server:   stun.l.google.com:19302
    SetServerName("stun.l.google.com");
    SetServerPort(19302);
    TestConnection();
    

    //raise(SIGINT);

    

    return 0;
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
