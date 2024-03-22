#include "..\include\ReadRequest.h"

#include <fstream>
#include <iostream>
#include <vector>
#include "include/Response.h"

ReadRequest::ReadRequest(int uniqueID, int opcode, const std::string& pathName,
                         int offset, int numBytesToRead)
    : Request(uniqueID, opcode, pathName),
      offset(offset),
      numBytesToRead(numBytesToRead) {}

// returns buffer, if empty failure in reading file
std::vector<char> readFile(const std::string& filePath, int offset,
                           int numBytesToRead) {
    // initialise buffer
    std::vector<char> buffer(numBytesToRead);

    // opens file in binary stream
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return buffer;  // Return an empty buffer indicating failure
    }

    // Seek to the specified offset
    file.seekg(offset);

    // Read the specified number of bytes into the buffer
    file.read(buffer.data(), numBytesToRead);

    // Close the file
    file.close();

    return buffer;
}

void printBuffer(const std::vector<char>& buffer) {
    for (char c : buffer) {
        std::cout << c;
    }
    std::cout << std::endl;
}

Response ReadRequest::process() {
    // initialise buffer
    std::vector<char> buffer(ReadRequest::numBytesToRead);
    // read file path
    buffer = readFile(ReadRequest::pathName, ReadRequest::offset,
                      ReadRequest::numBytesToRead);

    std::cout << "Processed ReadRequest for " << pathName << " with offset "
              << offset << " and numBytesToRead " << numBytesToRead
              << std::endl;

    printBuffer(buffer);
    // if buffer is empty, status=0
    int status = buffer.empty() ? 0 : 1;
    // assign timeModified to current time
    auto now = std::chrono::system_clock::now();
    long timeModified = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    //convert vector char to string
    std::string data(buffer.begin(), buffer.end());
    return Response(ReadRequest::uniqueID,status,timeModified,data);
}

// // check if file is being monitored by checking if hashmap contains this path
// bool ReadRequest::checkMonitor(HashMap& hashMap) {
//     // format the filepath to relative
//     // check if file is being monitored
//     if (hashMap.contains(ReadRequest::pathName)) {
//         std::cout << "File " << pathName << " is being monitored" << std::endl;
//         return true;
//     } else {
//         std::cout << "File " << pathName << " is not being monitored"
//                   << std::endl;
//         return false;
//     }
// }