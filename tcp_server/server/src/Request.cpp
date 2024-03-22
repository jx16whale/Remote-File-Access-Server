#include "..\include\Request.h"
#include <iostream>

Request::Request(int uniqueID, int opcode, const std::string& pathName)
    : uniqueID(uniqueID), opcode(opcode), pathName(pathName) {
        setPathName();
    }

Response Request::process() {
    // Implement processing
    std::cout << "Processing for " << pathName << std::endl;
};

void Request::setPathName() {
    // Implement setting pathName
    pathName = "./file_sys/"+pathName;
    std::cout << "Setting pathName for " << pathName << std::endl;
};