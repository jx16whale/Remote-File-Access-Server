#ifndef DELETEREQUEST_H
#define DELETEREQUEST_H

#include "Request.h"
#include "Response.h"

class DeleteRequest : public Request {

public:
    int offset;
    int numBytesToDel;
    DeleteRequest(int uniqueID, int opcode, const std::string& pathName, int offset, int numBytesToDel);
    // declaration for the destructor of the ReadRequest class.
    virtual ~DeleteRequest() = default;
    // This declares a member function named process that takes no arguments and returns void. The virtual keyword indicates that this function is a virtual function. Virtual functions are used in polymorphism, allowing derived classes to provide their own implementation of the function.
    Response process() override;
};

#endif // READREQUEST_H
