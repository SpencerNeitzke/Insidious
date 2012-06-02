#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

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

	SOCKET client;

	client = socket( AF_INET, SOCK_STREAM, 0 );

	sockaddr_in sin;

	memset( &sin, 0, sizeof sin );

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("173.26.250.104");
	sin.sin_port = htons(5555);

	if ( connect(client, (SOCKADDR*)(&sin), sizeof sin ) == SOCKET_ERROR ){
		return FALSE;
	}


}