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
      bytesToWrite(bytesToWrite) {
        std::cout << "WriteReq constructor called" << std::endl;
        std::cout << "Offset: " << offset << std::endl;
        std::cout << "Bytes to write: " << bytesToWrite << std::endl;

      }
// TODO: if offset larger than size of file then reject
bool writeFile(const std::string& filePath, int offset,const std::string bytesToWrite) {
    // Open the file in binary mode for reading and writing
    std::fstream file(filePath, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file " << filePath << std::endl;
        return false;
    }

    // Seek to the end of the file
    file.seekp(0, std::ios::end);
    std::streampos endPos = file.tellp();

    // Read contents from offset to end of file
    std::string fileContents;
    if (endPos > offset) {
        file.seekp(offset);
        char buffer[1024];
        std::streamsize bytesRead;
        while ((bytesRead = file.readsome(buffer, sizeof(buffer))) > 0) {
            fileContents.append(buffer, bytesRead);
        }
    }
    // if offset<sizeoffile return false TODO: test this
    if (offset > endPos) {
        std::cerr << "Error: Offset is beyond the file size." << std::endl;
        file.close();
        return false;
    }


    // Move back to the insert position
    file.seekp(offset);

    // Write the bytes to insert
    file.write(bytesToWrite.data(), bytesToWrite.size());

    // Write back the contents after the inserted bytes
    if (!fileContents.empty()) {
        file.write(fileContents.data(), fileContents.size());
    }

    // Check for any write errors
    if (file.fail()) {
        std::cerr << "Error: Failed to insert into file " << filePath << std::endl;
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
    if (fileContents.empty() and intStatus == 0) {
        std::cout << "ERROR: File Name provided is invalid." << std::endl;
        fileContents = "ERROR: File Name provided is invalid.";
    }else if (intStatus==0){
        fileContents = "ERROR: Failed to write to file.";
        std::cout << "ERROR: Failed to write to file." << std::endl;
    }else {
        std::cout << "Successfully wrote to file." << std::endl;
    }

    return Response(uniqueID, intStatus,timeModified, fileContents);
}