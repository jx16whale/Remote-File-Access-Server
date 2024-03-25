#ifndef ATTRREQUEST_H
#define ATTRREQUEST_H

#include "Request.h"
#include <vector>

class AttrRequest : public Request {

public:
    AttrRequest(int uniqueID, int opcode, const std::string& pathName);
    // declaration for the destructor of the WriteRequest class.
    virtual ~AttrRequest() = default;
    // This declares a member function named process that takes no arguments and returns void. The virtual keyword indicates that this function is a virtual function. Virtual functions are used in polymorphism, allowing derived classes to provide their own implementation of the function.
    Response process() override;
};

#endif // WRITEREQUEST_H
