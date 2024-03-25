#include "..\include\WriteRequest.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <sstream>

WriteRequest::WriteRequest(int uniqueID, int opcode, const std::string& pathName, int offset,std::string bytesToWrite)
    : Request(uniqueID, opcode, pathName),
      offset(offset),
      bytesToWrite(bytesToWrite) {}

bool writeFile(const std::string& filePath, int offset,const std::string bytesToWrite) {
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

std::string readFile3(const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return ""; // Return an empty string if unable to open file
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // Read the entire file into the stringstream

    // Close the file
    file.close();

    // Return the contents of the stringstream as a string
    return buffer.str();
}

// return response object with bytestowrite as data , look at status to know if successful or not
Response WriteRequest::process() {
    // Implement processing
    bool status = writeFile(WriteRequest::pathName, WriteRequest::offset,
              WriteRequest::bytesToWrite);
    std::cout << "Processed Write for " << pathName << " with offset " << offset
              << " and sequence " << std::endl;
    // convert bool status to int status
    int intStatus = status ? 1 : 0;
    
    // assign timeModified to current time
    long timeModified = getLastModifiedTime();

    std::string fileContents = readFile3(pathName);
    if (!fileContents.empty()) {
        std::cout << "File contents:" << std::endl;
        std::cout << fileContents << std::endl;
    } else {
        std::cout << "Failed to read file." << std::endl;
    }

    return Response(uniqueID, intStatus,timeModified, fileContents);
}