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
        // std::cout << "offset: " << offset << std::endl;
        // std::cout << "numBytesToRead: " << numBytesToRead << std::endl;
      }

std::vector<char> readFile2(const std::string& filePath) {
    // Initialise buffer
    std::vector<char> buffer;

    // Open file in binary stream
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return buffer;  // Return an empty buffer indicating failure
    }

    // Get the size of the file
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Resize buffer to fit the file contents
    buffer.resize(fileSize);

    // Read file contents into the buffer
    file.read(buffer.data(), fileSize);

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
    buffer = readFile2(ReadRequest::pathName);

    std::cout << "Processed ReadRequest for " << pathName << " with offset "
              << offset << " and numBytesToRead " << numBytesToRead
              << std::endl;

    // if buffer is empty, status=0
    int status = buffer.empty() ? 0 : 1;

    long timeModified = getLastModifiedTime();

    //convert vector char to string
    std::string data(buffer.begin(), buffer.end());
    if (status == 0) {
        data = "ERROR: Failed to read file.";
        std::cout << "ERROR: Failed to read file." << std::endl;
    }
    return Response(uniqueID,status,timeModified,data);
}