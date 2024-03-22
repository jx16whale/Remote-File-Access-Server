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

#include ".\include\ConnectionModule.h"
#include ".\include\ListRequest.h"
#include ".\include\MonitorRequest.h"
#include ".\include\ReadRequest.h"
#include ".\include\WriteRequest.h"
#include ".\include\marshaller.h"
#include ".\include\unmarshaller.h"
#include "include/Response.h"

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

Request unmarshallRequest(uint8_t *requestBuffer) {
    Request req(123, 1, "test.txt");
    // Change the parameters name as you like
    std::cout << "Debug:" << std::endl;

    req.uniqueID = Unmarshaller::unmarshallInt(&requestBuffer);
    std::cout << "UniqueId: " << req.uniqueID << std::endl;
    req.opcode = Unmarshaller::unmarshallInt(&requestBuffer);
    // print req opcode
    // std::cout << "IN UNMARSHALL Request opcode: " << req.opcode <<
    // std::endl;
    std::string filename = Unmarshaller::unmarshallString(&requestBuffer);

    // Switch for specific request to unmarshall
    switch (req.opcode) {
        case 1: {
            ReadRequest ReadRequest(
                req.uniqueID, req.opcode, filename,
                Unmarshaller::unmarshallInt(&requestBuffer),
                Unmarshaller::unmarshallInt(&requestBuffer));
            std::cout << "Received data from client: " << ReadRequest.opcode
                      << std::endl;
            std::cout << "Received data from client: " << ReadRequest.uniqueID
                      << std::endl;
            std::cout << "Received data from client: " << ReadRequest.pathName
                      << std::endl;

            // delete[] bufferPtr;
            delete[] requestBuffer;  // Not sure delete from where but need
                                     // to delete[] the uint8_t buffer
            return ReadRequest;
        }
            // case 5: {
            //     // add code to initialise the request object and return
            //     it req.offset =
            //     Unmarshaller::unmarshallInt(requestBuffer);
            //     req.bytesToReadFrom =
            //     Unmarshaller::unmarshallInt(requestBuffer);

            //     return req;
            // }

        case 2: {
            // add code to create back the structure and return it
            int offset = Unmarshaller::unmarshallInt(&requestBuffer);
            std::string bytesToWrite =
                Unmarshaller::unmarshallString(&requestBuffer);
            WriteRequest WriteReq(req.uniqueID, req.opcode, filename, offset,
                                  bytesToWrite);
            // print uniqueid, opcode, filename, offset, bytestowrite
            // std::cout << "IN UNMARSHAL WriteRequest uniqueID: " <<
            // WriteReq.uniqueID << std::endl; std::cout << "IN UNMARSHAL
            // WriteRequest opcode: " << WriteReq.opcode << std::endl;
            // std::cout
            // << "IN UNMARSHAL WriteRequest filename: " << filename <<
            // std::endl; std::cout << "IN UNMARSHAL WriteRequest
            // bytestowrite: " << bytesToWrite << std::endl;
            // // print typeid write req
            // std::cout << "IN SWITCH WriteRequest type: " <<
            // typeid(WriteReq).name() << std::endl;
            return WriteReq;
        }

        case 3: {
            // add code to create back the structure and return it
            int interval = Unmarshaller::unmarshallInt(&requestBuffer);
            MonitorRequest MonitorReq(req.uniqueID, req.opcode, filename,
                                      interval);
            return MonitorReq;
        }

        case 4: {
            ListRequest ListReq(req.uniqueID, req.opcode, filename);
            return ListReq;
        }

        default: {
            // std::cout << "IN UNMARSHAL SWITCH Error: opcode " <<
            // req.opcode
            // << " does not exist." << std::endl;
            break;
        }
    }
    // delete[] bufferPtr;
    delete[] requestBuffer;  // Not sure delete from where but need to
                             // delete[] the uint8_t buffer
    return req;
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
        Request requestObject = unmarshallRequest(bufferPtr);

        // initialise buffer to store return value from services
        std::vector<char> buffer(MAX_RESPONSE_BUFFER_LENGTH);

        Response responseObject = requestObject.process();
        // Close the sockets

        // after going thru the server service

        // create response object
    }
    // closesocket(clientSocket);
    closesocket(serverSocket);

    // Cleanup Winsock
    WSACleanup();
}