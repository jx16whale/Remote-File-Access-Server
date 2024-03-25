#include "WriteRequest.h"
#include "Response.h"
#include <iostream>

void main(){
    WriteRequest r(123, 2, "file.txt", 0, "Hello World");
    Response res = r.process();
    std::cout << "Response status: " << res.status << std::endl;
    std::cout << "Response timeModified: " << res.timeModified << std::endl;
    std::cout << "Response data: " << res.data << std::endl;
    std::cout << "Response reqid: " << res.requestId << std::endl;
}