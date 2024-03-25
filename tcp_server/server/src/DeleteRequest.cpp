#include "..\include\DeleteRequest.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include "..\include\Response.h"
#include <unistd.h>
#include <sys/stat.h>

DeleteRequest::DeleteRequest(int uniqueID, int opcode, const std::string& pathName,
                         int offset, int numBytesToDel)
    : Request(uniqueID, opcode, pathName),
      offset(offset),
      numBytesToDel(numBytesToDel) {
        std::cout << "DeleteReq constructor called" << std::endl;
      }

bool deleteContent(const std::string& filename, int offset, int sizeToDelete) {
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return false; // Return false if unable to open file
    }

    // Convert offset and sizeToDelete to streamoff and streamsize types
    std::streamoff offsetStream = static_cast<std::streamoff>(offset);
    std::streamsize sizeStream = static_cast<std::streamsize>(sizeToDelete);

    // Seek to the offset from the beginning of the file
    file.seekp(offsetStream);

    // Create a buffer to store the remaining content after the deleted portion
    char buffer;
    std::streampos currentPos = file.tellp();
    std::streampos endPos;

    // Read the remaining content after the deleted portion
    while (file.get(buffer)) {
        currentPos += 1; // Increment the current position
    }
    endPos = currentPos;

    // Move back to the start of the deleted portion
    file.seekp(offsetStream);

    // Move the remaining content to fill the deleted portion
    while (currentPos < endPos) {
        file.put(file.get());
        currentPos += 1; // Increment the current position
    }

    // Close the file
    file.close();

    // Truncate the file to remove the excess content using std::filesystem
    try {
        std::filesystem::resize_file(filename, offsetStream);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false; // Return false if an error occurs during truncation
    }

    return true; // Return true if successful
}

std::string readFile(const std::string& filename) {
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

// TODO test
Response DeleteRequest::process() {
    int status;
    // delete content from file
    if (deleteContent(DeleteRequest::pathName, DeleteRequest::offset, DeleteRequest::numBytesToDel)){
        status=1;
        std::cout << "DeleteRequest: Deleted content from " << pathName << " with offset ";
        }
    else{
        status=0;
        std::cout << "DeleteRequest: Failed to delete content from " << pathName << " with offset ";
    }


    std::cout << "Processed ReadRequest for " << pathName << " with offset "
              << offset << " and numBytestoDel " << numBytesToDel
              << std::endl;

    // assign timeModified to current time
    long timeModified = getLastModifiedTime();

    std::string fileContents = readFile(pathName);
    if (!fileContents.empty()) {
        std::cout << "File contents:" << std::endl;
        std::cout << fileContents << std::endl;
    } else {
        std::cout << "Failed to read file." << std::endl;
    }

    //convert vector char to string
    return Response(uniqueID,status,timeModified,fileContents);
}