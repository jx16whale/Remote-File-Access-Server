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

void readFileWithOffsetAndPrint(const std::string& filename, std::size_t offset, std::size_t sizeToDelete) {
    // Open the file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Find the size of the file
    file.seekg(0, std::ios::end);
    std::size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Ensure the offset is valid
    if (offset >= fileSize) {
        std::cerr << "Offset is beyond the file size." << std::endl;
        return;
    }

    // Read from byte 0 to the specified offset
    std::vector<char> combinedBuffer(fileSize-sizeToDelete);
    file.read(combinedBuffer.data(), offset);

    // Seek to the desired offset
    file.seekg(offset, std::ios::beg);

    // Skip the bytes to be deleted
    file.seekg(sizeToDelete, std::ios::cur);

    // Read the remaining content into a buffer
    file.read(combinedBuffer.data() + offset, fileSize - offset - sizeToDelete);

    // Print the buffer contents
    std::cout << "Contents of the file after skipping offset and deleting specified size:\n";
    std::cout.write(combinedBuffer.data(), combinedBuffer.size());

    // Close the file
    file.close();
}

bool deleteContent(const std::string& filename, int offset, int sizeToDelete) {
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return false; // Return false if unable to open file
    }

    // Move to the start offset
    file.seekp(offset);

    // Create a buffer to store the remaining content after the deleted portion
    char buffer;
    std::streampos currentPos = file.tellp();

    // Move the remaining content to fill the deleted portion
    while (file.get(buffer) && sizeToDelete > 0) {
        sizeToDelete--; // Decrease the size to delete
        currentPos+=1;   // Increment the current position
    }

    // Move back to the start of the deleted portion
    file.seekp(offset);

    // Move the remaining content to fill the deleted portion
    while (file.get(buffer)) {
        file.put(file.get());
        currentPos+=1; // Increment the current position
    }

    // Truncate the file to remove the excess content
    // file.close();
    // std::filesystem::resize_file(filename, currentPos);

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

// TODO test deleting too mch
Response DeleteRequest::process() {
    int status=1; // TODO change to 0 if failed
    readFileWithOffsetAndPrint(DeleteRequest::pathName, DeleteRequest::offset, DeleteRequest::numBytesToDel);
    // delete content from file
    // if (deleteContent(DeleteRequest::pathName, DeleteRequest::offset, DeleteRequest::numBytesToDel)){
    //     status=1;
    //     std::cout << "DeleteRequest: Deleted content from " << pathName << " with offset ";
    //     }
    // else{
    //     status=0;
    //     std::cout << "DeleteRequest: Failed to delete content from " << pathName << " with offset ";
    // }


    std::cout << "Processed DeleteRequest for " << pathName << " with offset "
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