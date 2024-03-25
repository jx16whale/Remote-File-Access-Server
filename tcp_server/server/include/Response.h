#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

class Response {
   public:
    int requestId;
    int status;
    long timeModified;
    std::string data;  // Either to return the data, error message or null?
    // Constructor to initialize members
    Response(int reqId, int stat, long timeMod, const std::string& d) :
        requestId(reqId), status(stat), timeModified(timeMod), data(d) {}
};

#endif  // RESPONSE_H
