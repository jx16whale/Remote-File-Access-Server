#include "..\include\ReadRequest.h"
#include "..\include\Response.h"

int main(){
    ReadRequest r(1, 2, "file.txt", 0, 10);
    Response res = r.process();
    std::cout << "Response status: " << res.status << std::endl;
    std::cout << "Response timeModified: " << res.timeModified << std::endl;
    std::cout << "Response data: " << res.data << std::endl;
    std::cout << "Response reqid: " << res.requestId << std::endl;
}