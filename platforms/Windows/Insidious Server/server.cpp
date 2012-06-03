#include <iostream>
#include <winsock2.h>
#include <process.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

void client(void* sock) {
	SOCKET Socket = (SOCKET)sock;
	std::cout<<"Client connected!\r\n\r\n";
	char *szMessage="Welcome to the server!\r\n";
	send(Socket,szMessage,strlen(szMessage),0);
}

int main()
{
	WSADATA WsaDat;
	if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0)
	{
		std::cout<<"WSA Initialization failed!\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(Socket==INVALID_SOCKET)
	{
		std::cout<<"Socket creation failed.\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	SOCKADDR_IN serverInf;
	serverInf.sin_family=AF_INET;
	serverInf.sin_addr.s_addr=INADDR_ANY;
	serverInf.sin_port=htons(55555);

	if(bind(Socket,(SOCKADDR*)(&serverInf),sizeof(serverInf))==SOCKET_ERROR)
	{
		std::cout<<"Unable to bind socket!\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	listen(Socket,1);
	SOCKET TempSock=SOCKET_ERROR;


	while(1){
			TempSock=accept(Socket,NULL,NULL);
			_beginthread(client, 0 , (void*)TempSock);
			TempSock = NULL;
	}


	// Shutdown our socket
	shutdown(Socket,SD_SEND);

	// Close our socket entirely
	closesocket(Socket);

	// Cleanup Winsock
	WSACleanup();
	system("PAUSE");
	return 0;
}
