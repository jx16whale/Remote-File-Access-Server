#include "include\Request.h"
#include "include\MonitorRequest.h"
#include "include\Response.h"
#include "include\HashMap.h"
#include "include\WriteRequest.h"
#include <iostream>

Request* unmarshallRequest3() {
    std::cout << "in unmarhsall" << std::endl;
    return new MonitorRequest(123,3,"file.txt",10);
}

Request* unmarshallReques2t() {
    std::cout << "in unmarhsall" << std::endl;
    return new WriteRequest(1234,2,"file.txt",10,"testing");
}
HashMap globalHashMap;

int main(){
    std::cout << "in main" << std::endl;
    // monitor req
    Request* requestPtr = unmarshallRequest3();
    Response resObj = requestPtr->process();
    // if monitor request, add to hashmap
    if (requestPtr->opcode == 3) {
        // Downcasting the pointer to the derived class pointer
        MonitorRequest* derivedPtr = dynamic_cast<MonitorRequest*>(requestPtr);

        // Check if the dynamic_cast was successful
        if (derivedPtr != nullptr) {
            // Now you can use the derived class's members using the derivedPtr
            // convert uniqueID to string
            std::string uniqueIDStr = std::to_string(requestPtr->uniqueID);
            // // add to hashmap current file:[machineid, expiry time]
            globalHashMap.insert(requestPtr->pathName, uniqueIDStr,derivedPtr->expiryTime);

            std::cout << "File " << requestPtr->pathName << " is being monitored" << std::endl;
        } else {
            std::cerr << "Error: Unable to downcast pointer." << std::endl;
        }
        
    }

    requestPtr = unmarshallReques2t();

    // WRITE or DELETE need to check hashmap
        if (requestPtr->opcode == 2 || requestPtr->opcode == 5) {
            if (globalHashMap.contains(requestPtr->pathName)) {
                std::cout << "File " << requestPtr->pathName << " is being monitored" << std::endl;
                // get machineid
                int reqid = std::stoi(globalHashMap.getValue(requestPtr->pathName));
                // send alert to monitoring process AKA send TODO
                Response alertResponse = Response(reqid,resObj.status,resObj.timeModified,resObj.data);
                // print debug + alert.data
                std::cout << "Alert data: " << alertResponse.data << std::endl;
                // TODO send alertresponse back thru marshaller and socket
            } else {
                std::cout << "File " << requestPtr->pathName << " is not being monitored"
                        << std::endl;
            }
        }
    //print resobj.data
    std::cout<< resObj.data<< std::endl;
    return 0;
}