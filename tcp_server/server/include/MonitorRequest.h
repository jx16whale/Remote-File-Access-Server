#ifndef MONITORREQUEST_H
#define MONITORREQUEST_H

#include "Request.h"
#include <vector>
#include <chrono>
#include "HashMap.h"

class MonitorRequest : public Request {

public:
    int lengthOfInterval;
    std::chrono::system_clock::time_point expiryTime;
    MonitorRequest(int uniqueID, int opcode, const std::string& pathName, int lengthOfInterval);
    // declaration for the destructor of the MonitorRequest class.
    virtual ~MonitorRequest() = default;
    // This declares a member function named process that takes no arguments and returns void. The virtual keyword indicates that this function is a virtual function. Virtual functions are used in polymorphism, allowing derived classes to provide their own implementation of the function.
    Response process() override;
};

#endif // MONITORREQUEST_H
