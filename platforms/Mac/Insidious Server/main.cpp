//
//  main.cpp
//  Insidious Server
//

#include <iostream>
#include <sys/types.h>   // Types used in sys/socket.h and netinet/in.h
#include <netinet/in.h>  // Internet domain address structures and functions
#include <sys/socket.h>  // Structures and functions used for socket API
#include <pthread.h>
#include <stdio.h>

void *client(void* sock) {
    int* data = reinterpret_cast<int*>(sock);
    int Socket = *data;
    
	std::cout << "Client connected!\n";
	char *szMessage = "Welcome to the server!\n";
	send(Socket, szMessage, strlen(szMessage), 0);
    pthread_exit(0);
}

int main()
{
    int socketHandle;
    
    // create socket
    if((socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
    {
        fprintf(stderr, "Could not create socket handle.\n");
        close(socketHandle);
        return 0;
    }
	
	struct sockaddr_in serverInf;
	serverInf.sin_family = AF_INET;
	serverInf.sin_addr.s_addr = INADDR_ANY;
	serverInf.sin_port = htons(55555);
    
	if(bind(socketHandle, (sockaddr*)&serverInf, sizeof(serverInf)) < 0)
    {
        fprintf(stderr, "Could not bind to socket.\n");
        close(socketHandle);
        return 0;
    }
    
	listen(socketHandle, 1);
	int TempSock;
    pthread_t clientThread;
    
    fprintf(stderr, "Listening for connections...\n");
	while(1) {
        clientThread = NULL;
        TempSock = accept(socketHandle, NULL, NULL);
        pthread_create(&clientThread, NULL, client, &TempSock);
        TempSock = NULL;
	}
    
	// Shutdown our socket
	shutdown(socketHandle, SHUT_RDWR);
    
	// Close our socket entirely
	close(socketHandle);
    
	return 1;
}