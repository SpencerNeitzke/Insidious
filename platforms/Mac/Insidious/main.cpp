//
//  main.cpp
//  Insidious
//

#include <iostream>
#include <pthread.h>
#import "connect.hpp"
#import "usb.hpp"

using namespace std;

static void SignalHandler(int sigraised)
{
    fprintf(stderr, "\nInterrupted.\n");
    exit(0);
}

int main (int argc, const char * argv[])
{
    sig_t oldHandler = signal(SIGINT, SignalHandler);
    if (oldHandler == SIG_ERR) {
        // Could not establish new signal handler.
    }
    
    pthread_t socketThread;
    pthread_create(&socketThread, NULL, &SocketConnect::openSocket, NULL);

    USBScanner *usb = new USBScanner();
    usb->scan();
    
	return 0;
}