#include "..\include\MonitorRequest.h"
#include <sys/stat.h>
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
    std::cout << "Processed Monitor for " << pathName << std::endl;
    long timeModified = getLastModifiedTime();
    return Response(MonitorRequest::uniqueID, 1, timeModified, "MONITORING");
}