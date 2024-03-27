#include "..\include\ReadRequest.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include "..\include\Response.h"
#include <sys/stat.h>

ReadRequest::ReadRequest(int uniqueID, int opcode, const std::string& pathName,
                         int offset, int numBytesToRead)
    : Request(uniqueID, opcode, pathName),
      offset(offset),
      numBytesToRead(numBytesToRead) {
        std::cout << "ReadRequest constructor called" << std::endl;
        std::cout << "offset: " << offset << std::endl;
        std::cout << "numBytesToRead: " << numBytesToRead << std::endl;
      }

// returns buffer, if empty failure in reading file
std::vector<char> readFile2(const std::string& filePath, int offset,
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
    buffer = readFile2(ReadRequest::pathName, ReadRequest::offset,
                      ReadRequest::numBytesToRead);

    std::cout << "Processed ReadRequest for " << pathName << " with offset "
              << offset << " and numBytesToRead " << numBytesToRead
              << std::endl;

    // if buffer is empty, status=0
    int status = buffer.empty() ? 0 : 1;

    long timeModified = getLastModifiedTime();

    //convert vector char to string
    std::string data(buffer.begin(), buffer.end());
    return Response(uniqueID,status,timeModified,data);
}