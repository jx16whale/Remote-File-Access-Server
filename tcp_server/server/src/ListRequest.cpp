#include "..\include\ListRequest.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "..\include\Response.h"
#include <sys/stat.h>

ListRequest::ListRequest(int uniqueID, int opcode, const std::string& pathName)
    : Request(uniqueID, opcode, pathName) {
        std::cout << "ListRequest constructor called" << std::endl;
    }


std::string listDirectory(const std::string& dirPath) {
    std::string result;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            result += entry.path().string() + "\n";
        }
    } catch (const std::filesystem::filesystem_error& e) {
        result = "Error: " + std::string(e.what());
    }

    return result;
}

Response ListRequest::process() {
    // Implement processing
    std::string result = listDirectory(ListRequest::pathName);
    std::cout << "Listed for " << pathName << std::endl;
    long timeModified = getLastModifiedTime();
    return Response(ListRequest::uniqueID, 1, timeModified, result);
}