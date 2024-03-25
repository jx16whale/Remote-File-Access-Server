#include "..\include\MonitorRequest.h"

#include <fstream>
#include <iostream>
#include <vector>

MonitorRequest::MonitorRequest(int uniqueID, int opcode,
                               const std::string& pathName,
                               int lengthOfInterval)
    : Request(uniqueID, opcode, pathName), lengthOfInterval(lengthOfInterval) {
    std::chrono::system_clock::time_point now =
        std::chrono::system_clock::now();
    std::chrono::system_clock::duration interval =
        std::chrono::seconds(lengthOfInterval);
    expiryTime = now + interval;  // Calculating expiry time
}

Response MonitorRequest::process() {
    // // convert uniqueID to string
    // std::string uniqueIDStr = std::to_string(uniqueID);
    // // add to hashmap current file:[machineid, expiry time]
    // hashMap.insert(MonitorRequest::pathName, uniqueIDStr,
    //                MonitorRequest::expiryTime);
    std::cout << "Processed Monitor for " << pathName << std::endl;
}