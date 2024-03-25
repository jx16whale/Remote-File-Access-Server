#include "..\include\Request.h"
#include <iostream>

Request::Request(int uniqueID, int opcode, const std::string& pathName)
    : uniqueID(uniqueID), opcode(opcode), pathName(pathName) {
        setPathName();
    }

Response Request::process() {
    // Implement processing
    std::cout << "SHOULD NOT BE HERE " << pathName << std::endl;
    return Response(123,1,123,"wrong");
};

void Request::setPathName() {
    // Implement setting pathName
    pathName = "C:\\Users\\chunw\\ForBen\\tcp_server\\file_sys\\"+pathName;
    std::cout << "Setting pathName for " << pathName << std::endl;
};