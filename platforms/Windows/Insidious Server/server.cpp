#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <iostream>

using namespace std;

int main() {
	WSADATA wsaData;
	WORD version;
	int error;

	version = MAKEWORD(2, 0);

	error = WSAStartup(version, &wsaData);

	if ( error != 0 )
	{
		return FALSE;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		 HIBYTE( wsaData.wVersion ) != 0 )
	{
		WSACleanup();
		return FALSE;
	}


	SOCKET server;

	server = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in sin;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(5555);

	if (bind( server, (SOCKADDR*)&sin, sizeof(sin) ) == SOCKET_ERROR ){
		return FALSE;
	}

	while ( listen( server, SOMAXCONN ) == SOCKET_ERROR );

	SOCKET client;
	int length;

	while(1) {
		length = sizeof sin;
		client = accept( server, (SOCKADDR*)&sin, &length );
		cout << "Client connected" << endl;
		cout << "Sending Instructions..." << endl;
	}

}