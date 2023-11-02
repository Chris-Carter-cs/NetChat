// NetChat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <csignal>

std::thread* thread_listener;
std::thread* thread_speaker;


int ListenerThread();
int SenderThread();

void signal_handler(int sig);

int ListenerThread() {
    printf("\tThread 1 Complete\n");
    return 0;
}

int SenderThread() {
    printf("\tThread 2 Complete\n");
    return 0;
}

void signal_handler(int sig) {
    printf("Signal Handler called. Joining threads...\n");
    thread_listener->join();
    thread_speaker->join();

    printf("Threads joined. Deleting thread objects...\n");

    delete thread_listener;
    delete thread_speaker;

    printf("Finished. Exiting.\n");

    exit(0);
}

int main()
{
    printf("Setting signal handler\n");

    signal(SIGINT, signal_handler);

    std::cout << "Initilizing Threads...\n";

    thread_listener = new std::thread(ListenerThread);
    thread_speaker = new std::thread(SenderThread);

    raise(SIGINT);
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
