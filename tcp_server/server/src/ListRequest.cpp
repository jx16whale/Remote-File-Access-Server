#include "..\include\ListRequest.h"

#include <fstream>
#include <iostream>
#include <vector>
#include "include\Response.h"

ListRequest::ListRequest(int uniqueID, int opcode, const std::string& pathName)
    : Request(uniqueID, opcode, pathName) {}

// TODO: decide implementation need to return what
// void listDirectory(const std::string& filePath) {

// }

Response ListRequest::process() {
    // Implement processing
    std::cout << "Listed for " << pathName << std::endl;
}