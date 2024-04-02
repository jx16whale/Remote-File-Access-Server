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
#include <ostream>

DeleteRequest::DeleteRequest(int uniqueID, int opcode, const std::string& pathName,
                         int offset, int numBytesToDel)
    : Request(uniqueID, opcode, pathName),
      offset(offset),
      numBytesToDel(numBytesToDel) {
        std::cout << "DeleteReq constructor called" << std::endl;
        std::cout << "Offset: " << offset << std::endl;
        std::cout << "numBytesToDel: " << numBytesToDel << std::endl;
      }

bool readFileWithOffsetAndPrint(const std::string& filename, std::size_t offset, std::size_t sizeToDelete) {
    // Open the file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    // Find the size of the file
    file.seekg(0, std::ios::end);
    std::size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Ensure the offset is valid
    if (offset >= fileSize) {
        std::cerr << "Offset is beyond the file size." << std::endl;
        return false;
    }

    // Read from byte 0 to the specified offset
    std::vector<char> combinedBuffer(fileSize-sizeToDelete);
    file.read(combinedBuffer.data(), offset);

    // Seek to the desired offset
    file.seekg(offset, std::ios::beg);

    // Skip the bytes to be deleted
    file.seekg(sizeToDelete, std::ios::cur);

    // Read the remaining content
    file.read(combinedBuffer.data() + offset, fileSize - offset - sizeToDelete);

    // Close the file
    file.close();

    // // Print the buffer contents
    // std::cout << "Contents of the file after skipping offset and deleting specified size:\n";
    // std::cout.write(combinedBuffer.data(), combinedBuffer.size());

    // Open the file for writing
    std::ofstream outFile(filename); // Open in text mode
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return false;
    }

    // Write the modified content back to the file
    outFile.write(combinedBuffer.data(), combinedBuffer.size());
    // Close the file
    outFile.close();
    return true;
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

Response DeleteRequest::process() {
    int status;

    if (readFileWithOffsetAndPrint(DeleteRequest::pathName, DeleteRequest::offset, DeleteRequest::numBytesToDel)){
        status=1;
        std::cout << "DeleteRequest: Deleted content from " << pathName << " with offset ";
        }
    else{
        status=0;
        std::cout << "DeleteRequest: Failed to delete content from " << pathName << " with offset ";
    };

    // assign timeModified to current time
    long timeModified = getLastModifiedTime();

    std::string fileContents = readFile(pathName);
    if (fileContents.empty() and status == 0) {
        std::cout << "Failed to read file." << std::endl;
        fileContents = "ERROR: Failed to read file.";
    }else if (status==0){
        fileContents = "ERROR: Failed to delete content from file.";
    }else{
        std::cout << "Successfully deleted content from file." << std::endl;
    }

    //convert vector char to string
    return Response(uniqueID,status,timeModified,fileContents);
}