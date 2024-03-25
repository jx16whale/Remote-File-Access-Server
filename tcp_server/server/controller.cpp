// #include <unistd.h>  // Include the <unistd.h> header file for the sleep
// function
#include <winsock2.h>

#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <utility>  // For std::pair
#include <vector>
#include "..\include\HashMap.h"
#include ".\include\ListRequest.h"
#include ".\include\MonitorRequest.h"
#include ".\include\ReadRequest.h"
#include ".\include\WriteRequest.h"
#include ".\include\DeleteRequest.h"
#include ".\include\marshaller.h"
#include ".\include\unmarshaller.h"
#include "include/Response.h"
#include <cstdint>

// controller listens to a port on start and waits for connection
// on connection
//     calls the marshaller mod -> return the request object
//     find out what type of request
//     process the request accordingly
//     pass the output to the marshaller
//     return to the ip addr
//     continues the connection listening loop

int MAX_RESPONSE_BUFFER_LENGTH = 100;

// create global variable globalhashmap
HashMap globalHashMap;

Request* unmarshallRequest(uint8_t *requestBuffer) {
    // Change the parameters name as you like
    std::cout << "Debug:" << std::endl;
    int opcode = Unmarshaller::unmarshallInt(&requestBuffer);

    // Switch for specific request to unmarshall
    switch (opcode) {
        case 1: {
            return new ReadRequest(Unmarshaller::unmarshallInt(&requestBuffer),
                                   opcode,
                                   Unmarshaller::unmarshallString(&requestBuffer),
                                   Unmarshaller::unmarshallInt(&requestBuffer),
                                   Unmarshaller::unmarshallInt(&requestBuffer));
        }
        case 2:{
            return new WriteRequest(Unmarshaller::unmarshallInt(&requestBuffer),
                                    opcode,
                                    Unmarshaller::unmarshallString(&requestBuffer),
                                    Unmarshaller::unmarshallInt(&requestBuffer),
                                    Unmarshaller::unmarshallString(&requestBuffer));
        }
        case 3:{
            return new MonitorRequest(Unmarshaller::unmarshallInt(&requestBuffer),
                                    opcode,
                                    Unmarshaller::unmarshallString(&requestBuffer),
                                    Unmarshaller::unmarshallInt(&requestBuffer));
        }
        case 4:{
            return new ListRequest(Unmarshaller::unmarshallInt(&requestBuffer),
                                    opcode,
                                    Unmarshaller::unmarshallString(&requestBuffer));
        }
        case 5:{
            return new DeleteRequest(Unmarshaller::unmarshallInt(&requestBuffer),
                                   opcode,
                                   Unmarshaller::unmarshallString(&requestBuffer),
                                   Unmarshaller::unmarshallInt(&requestBuffer),
                                   Unmarshaller::unmarshallInt(&requestBuffer));
        }
            
        default: {
            std::cout << "IN UNMARSHAL SWITCH Error: opcode " << opcode << " does not exist." << std::endl;
            break;
        }
    }
}

void marshallReply(Response reply, uint8_t **replyBuffer) {
    // Change the pararmeter names as you see fit
    int requestId = reply.requestId;
    int status = reply.status;
    long timeModified = reply.timeModified;
    std::string data = reply.data;
    int paddingSize =
        Marshaller::calculatePadding(static_cast<long long>(data.length()));

    int estimatedSize = sizeof(int) * 3;
    estimatedSize += sizeof(int) + data.length();
    estimatedSize += paddingSize;
    *replyBuffer = new uint8_t[estimatedSize];

    Marshaller::marshallInt(replyBuffer, requestId);
    Marshaller::marshallInt(replyBuffer, status);
    Marshaller::marshallLong(replyBuffer, timeModified);
    Marshaller::marshallString(replyBuffer, paddingSize, data.c_str());
}

int main() {
    // CONNECTION
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "socket creation failed with error: " << WSAGetLastError()
                  << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to an address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr =
        htonl(INADDR_ANY);  // Accept connections from any address
    serverAddress.sin_port = htons(8080);  // Use port 8080 for example
    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress),
             sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError()
                  << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    // END OF CONNECTION PART

    while (true) {
        std::cout << "Waiting for incoming connection..." << std::endl;

        // Receive data
        char receiveBuffer[1024];  // Assuming a maximum buffer size of 1024
                                   // bytes

        int bytesReceived = recvfrom(serverSocket, receiveBuffer,
                                     sizeof(receiveBuffer) - 1, 0, NULL, NULL);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recv failed with error: " << WSAGetLastError()
                      << std::endl;
        } else {
            std::cout << "Received " << bytesReceived << " bytes: ";
            for (int i = 0; i < bytesReceived; ++i) {
                std::cout << std::hex << (int)receiveBuffer[i] << " ";
            }
            std::cout << std::endl;
        }
        uint8_t bytesArray2[sizeof(receiveBuffer)];
        for (int i = 0; i < sizeof(receiveBuffer); i++) {
            bytesArray2[i] = receiveBuffer[i];
        }

        uint8_t *bufferPtr = bytesArray2;
        Request* requestPtr = unmarshallRequest(bufferPtr);

        // initialise buffer to store return value from services
        std::vector<char> buffer(MAX_RESPONSE_BUFFER_LENGTH);

        Response responseObject = requestPtr->process();
        // Close the sockets

        // after going thru the server service

        // create response object
    }
    // closesocket(clientSocket);
    closesocket(serverSocket);

    // Cleanup Winsock
    WSACleanup();
}