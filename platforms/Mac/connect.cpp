//
//  connect.cpp
//  Insidious
//

#import "connect.hpp"
#include <iostream>
#include <sys/types.h>   // Types used in sys/socket.h and netinet/in.h
#include <netinet/in.h>  // Internet domain address structures and functions
#include <sys/socket.h>  // Structures and functions used for socket API
#include <netdb.h>       // Used for domain/DNS hostname lookup
#include <unistd.h>
#include <errno.h>

void SocketConnect::openSocket() {
    int socketHandle;
    
    // create socket
    if((socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
    {
        fprintf(stderr, "Could not create socket handle.\n");
        close(socketHandle);
        return;
    }
    
	struct sockaddr_in remoteSocketInfo;
	struct hostent *hPtr;

	char *remoteHost = "173.26.250.104";
	int portNumber = 5555;
    
	bzero(&remoteSocketInfo, sizeof(sockaddr_in));  // Clear structure memory
    
	// Get system information
	if((hPtr = gethostbyname(remoteHost)) == NULL)
	{
        fprintf(stderr, "System DNS name resolution not configured properly.\n");
        fprintf(stderr, "Error number: %d\n", ECONNREFUSED);
        return;
	}
    
	// Load system information for remote socket server into socket data structures
	memcpy((char *)&remoteSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
	remoteSocketInfo.sin_family = AF_INET;
	remoteSocketInfo.sin_port = htons((u_short)portNumber);      // Set port number
    
    fprintf(stderr, "Attempting socket connection...\n");
    if(connect(socketHandle, (struct sockaddr *)&remoteSocketInfo, sizeof(sockaddr_in)) < 0)
    {
        fprintf(stderr, "Could not open socket.\n");
        close(socketHandle);
        return;
    }
    
    // Socket connected
    fprintf(stderr, "Socket connected.\n");
    // Do stuff...
    
    // Finished with socket
    close(socketHandle);
}