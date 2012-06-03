#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

int main()
{
	
	WSADATA WsaDat;
	if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0)
	{
		std::cout<<"Winsock error - Winsock initialization failed\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	
	SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(Socket==INVALID_SOCKET)
	{
		std::cout<<"Winsock error - Socket creation Failed!\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	
	struct hostent *host;
	if((host=gethostbyname("localhost"))==NULL)
	{
		std::cout<<"Failed to resolve hostname.\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port=htons(8888);
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);

	if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))!=0)
	{
		std::cout<<"Failed to establish connection with server\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	
	char buffer[1000];
	memset(buffer,0,999);
	int inDataLength=recv(Socket,buffer,1000,0);
	std::cout<<buffer;

	
	shutdown(Socket,SD_SEND);
	closesocket(Socket);
	WSACleanup();
	system("PAUSE");
	return 0;
}