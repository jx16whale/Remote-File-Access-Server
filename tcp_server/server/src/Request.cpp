#include "..\include\Request.h"
#include <iostream>
#include <sys/stat.h>

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

long Request::getLastModifiedTime() {
    struct stat file_stat;
    const std::string& filename = pathName;
    // Use stat to retrieve file information
    if (stat(filename.c_str(), &file_stat) != 0) {
        std::cerr << "Error: Unable to get file information for " << filename << std::endl;
        return -1; // Return -1 to indicate error
    }

    // Return the last modification time as long integer
    return static_cast<long>(file_stat.st_mtime);
}