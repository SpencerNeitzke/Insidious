#include <iostream>
#include <sys/types.h>   // Types used in sys/socket.h and netinet/in.h
#include <netinet/in.h>  // Internet domain address structures and functions
#include <sys/socket.h>  // Structures and functions used for socket API
#include <netdb.h>       // Used for domain/DNS hostname lookup
#include <unistd.h>
#include <errno.h>

using namespace std;

main()
{
   int socketHandle;

   // create socket

   if((socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
   {
      close(socketHandle);
      exit(EXIT_FAILURE);
   }

	struct sockaddr_in remoteSocketInfo;
	struct hostent *hPtr;
	int socketHandle;
	char *remoteHost="173.26.250.104";
	int portNumber = 5555;

	bzero(&remoteSocketInfo, sizeof(sockaddr_in));  // Clear structure memory

	// Get system information

	if((hPtr = gethostbyname(remoteHost)) == NULL)
	{
	   cerr << "System DNS name resolution not configured properly." << endl;
	   cerr << "Error number: " << ECONNREFUSED << endl;
	   exit(EXIT_FAILURE);
	}

	// Load system information for remote socket server into socket data structures

	memcpy((char *)&remoteSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
	remoteSocketInfo.sin_family = AF_INET;
	remoteSocketInfo.sin_port = htons((u_short)portNumber);      // Set port number

	if( (connect(socketHandle, (struct sockaddr *)&remoteSocketInfo, sizeof(sockaddr_in)) < 0)
	{
	   close(socketHandle);
	   exit(EXIT_FAILURE);
	}

	close(socketHandle);
       
}
          