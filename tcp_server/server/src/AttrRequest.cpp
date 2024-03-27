#include "..\include\AttrRequest.h"
#include <iostream>
#include <sys/stat.h>
#include <string>
#include <ctime>
#include "..\include\Response.h"

AttrRequest::AttrRequest(int uniqueID, int opcode, const std::string& pathName)
    : Request(uniqueID, opcode, pathName) {
        std::cout << "AttrRequest constructor called" << std::endl;
    }

Response AttrRequest::process() {
    long timeMod = getLastModifiedTime();
    // print timemod
    std::cout << "Processed AttrRequest for " << pathName << " with time modified " << timeMod << std::endl;
    return Response(AttrRequest::uniqueID, 1, timeMod, "file time modified is in timemodified variable");
}