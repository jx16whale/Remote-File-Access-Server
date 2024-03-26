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
#include ".\include\AttrRequest.h"
#include "include/Response.h"
#include <cstdint>
#include <ws2tcpip.h>

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
std::unordered_map<int, char*> duplicateRecordHashMap;

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
        case 6:{
            return new AttrRequest(Unmarshaller::unmarshallInt(&requestBuffer),
                                   opcode,
                                   Unmarshaller::unmarshallString(&requestBuffer));
        }
            
        default: {
            std::cout << "IN UNMARSHAL SWITCH Error: opcode " << opcode << " does not exist." << std::endl;
            break;
        }
    }
    return nullptr;
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

    std::string input;
    bool recordReqReply;
    // Read input from standard input
    std::cout << "Enter 0 for AT MOST ONCE or 1 for AT LEAST ONCE: ";
    std::cin >> input;
    // Check if the input is "0" or "1"
    if (input == "0") {
        std::cout << "Input is 0." << std::endl;
        recordReqReply = true;
    } else if (input == "1") {
        std::cout << "Input is 1." << std::endl;
        recordReqReply = false;
    } else {
        std::cout << "Invalid input." << std::endl;
        return 0;
    }

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
    sockaddr_in serverAddress, clientAddress;
    socklen_t len = sizeof(clientAddress);  //len is value/result 
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
        // at least once no record is taken
        std::cout << "Waiting for incoming connection..." << std::endl;

        // Receive data
        char receiveBuffer[1024];  // Assuming a maximum buffer size of 1024
                                   // bytes

        int bytesReceived = recvfrom(serverSocket, receiveBuffer,
                                     sizeof(receiveBuffer) - 1, 0, ( struct sockaddr *) &clientAddress, &len);

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
        
        // check if recordReqReply is true and if request.uniqueid is in duplicateRecordHashMap
        if (recordReqReply && duplicateRecordHashMap.find(requestPtr->uniqueID) != duplicateRecordHashMap.end()) {
            // if true, skip the process() and send the response back to client
            // get the response from duplicateRecordHashMap
            char* duplicateResponse = duplicateRecordHashMap[requestPtr->uniqueID];
            // send response back to client
            sendto(serverSocket,duplicateResponse, strlen(duplicateResponse),0, (const struct sockaddr *) &clientAddress, len);
            std::cout << "Duplicate request found. Sending back the response to client." << std::endl;
            continue;
        }

        // initialise buffer to store return value from services
        std::vector<char> buffer(MAX_RESPONSE_BUFFER_LENGTH);

        // each request performs their own process() eg. write will write, read will read...
        Response responseObject = requestPtr->process();
        
        // if monitor request, add to hashmap
        if (requestPtr->opcode == 3) {
            // Downcasting the pointer to the derived class pointer
            MonitorRequest* derivedPtr = dynamic_cast<MonitorRequest*>(requestPtr);

            // Check if the dynamic_cast was successful
            if (derivedPtr != nullptr) {
                // Now you can use the derived class's members using the derivedPtr
                // convert uniqueID to string
                std::string uniqueIDStr = std::to_string(requestPtr->uniqueID);
                // // add to hashmap current file:[machineid, expiry time]
                globalHashMap.insert(requestPtr->pathName, uniqueIDStr,derivedPtr->expiryTime);
                std::cout << "File " << requestPtr->pathName << " is being monitored" << std::endl;
            } else {
                std::cerr << "Error: Unable to downcast pointer." << std::endl;
            }
            
        }

        // WRITE or DELETE need to check hashmap
        if (requestPtr->opcode == 2 || requestPtr->opcode == 5) {
            if (globalHashMap.contains(requestPtr->pathName)) {
                std::cout << "File " << requestPtr->pathName << " is being monitored" << std::endl;
                // get machineid
                int reqid = std::stoi(globalHashMap.getValue(requestPtr->pathName));
                // create alert response
                Response alertResponse = Response(reqid,responseObject.status,responseObject.timeModified,responseObject.data);
                // TODO AFTER CFM SEND SUCCESS send alertresponse back thru marshaller and socket
                // print debug + alert.data
                std::cout << "Alert data: " << alertResponse.data << std::endl;
            } else {
                std::cout << "File " << requestPtr->pathName << " is not being monitored"
                        << std::endl;
            }
        }

        // TODO TEST send response thru marshaller and socket
        uint8_t* bufferPtrResponse;
        marshallReply(responseObject, &bufferPtrResponse);
        char* charPtr;
        for (int i = 0; i < sizeof(bufferPtrResponse); i++) {
            charPtr[i] = bufferPtrResponse[i];
        }
        // TODO before sending save it as a record in duplicateRecordHashMap
        if (recordReqReply) {
            duplicateRecordHashMap[requestPtr->uniqueID] = charPtr;
            std::cout << "Request response saved in duplicateRecordHashMap" << std::endl;
        }
        sendto(serverSocket,charPtr, strlen(charPtr),0, (const struct sockaddr *) &clientAddress, len);
    }
    // closesocket(clientSocket);
    closesocket(serverSocket);

    // Cleanup Winsock
    WSACleanup();
}