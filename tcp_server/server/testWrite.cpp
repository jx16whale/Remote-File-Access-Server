#include "include\Request.h"
#include "include\DeleteRequest.h"
#include "include\Response.h"
#include <iostream>

Request* unmarshallRequest4() {
    std::cout << "in unmarhsall" << std::endl;
    return new DeleteRequest(123,2,"file.txt",0,1);
}

int main(){
    std::cout << "in main" << std::endl;
    // monitor req
    Request* requestPtr = unmarshallRequest4();
    Response resObj = requestPtr->process();
    
    //print resobj.data
    std::cout<< resObj.data<< std::endl;
    return 0;
}