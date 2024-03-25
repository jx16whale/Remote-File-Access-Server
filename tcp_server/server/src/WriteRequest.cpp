#include "..\include\WriteRequest.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>

WriteRequest::WriteRequest(int uniqueID, int opcode,
                           const std::string& pathName, int offset,
                           std::string bytesToWrite)
    : Request(uniqueID, opcode, pathName),
      offset(offset),
      bytesToWrite(bytesToWrite) {}

bool writeFile(const std::string& filePath, int offset,
               const std::string bytesToWrite) {
    // Open the file in binary mode for writing
    std::ofstream file(filePath,
                       std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file " << filePath << std::endl;
        return false;
    }

    // Seek to the specified offset
    file.seekp(offset);

    // Write the bytes to the file
    file.write(bytesToWrite.data(), bytesToWrite.size());

    // Check for any write errors
    if (file.fail()) {
        std::cerr << "Error: Failed to write to file " << filePath << std::endl;
        file.close();
        return false;
    }

    // Close the file
    file.close();

    return true;
}
// return response object with bytestowrite as data , look at status to know if successful or not
Response WriteRequest::process() {
    // format the filepath to relative
    setPathName();
    // Implement processing for ReadRequest
    // std::vector<char> buffer(ReadRequest::numBytesToRead);
    // buffer = readFile(ReadRequest::pathName, ReadRequest::offset,
    // ReadRequest::numBytesToRead);

    // Implement processing
    bool status = writeFile(WriteRequest::pathName, WriteRequest::offset,
              WriteRequest::bytesToWrite);
    std::cout << "Processed Write for " << pathName << " with offset " << offset
              << " and sequence " << std::endl;
    // convert bool status to int status
    int intStatus = status ? 1 : 0;
    
    // assign timeModified to current time
    auto now = std::chrono::system_clock::now();
    long timeModified = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    return Response(uniqueID, intStatus,timeModified, WriteRequest::bytesToWrite);
}