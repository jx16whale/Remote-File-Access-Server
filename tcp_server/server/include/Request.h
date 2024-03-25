#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include "Response.h"

class Request {
   public:
    int opcode;
    int uniqueID;
    std::string pathName;
    Request(int uniqueID, int opcode, const std::string& pathName);
    // ~Request() = default;
    virtual Response process();
    void setPathName();
    long getLastModifiedTime();
};

#endif  // REQUEST_H
