#include "include\Request.h"
#include "include\ReadRequest.h"
#include "include\Response.h"
#include <iostream>

Request* unmarshallRequest() {
    std::cout << "in unmarhsall" << std::endl;
    return new ReadRequest(123,1,"file.txt",0,10);
}

int hello(){
    std::cout << "in main" << std::endl;
    Request* reqPtr = unmarshallRequest();

    Response resObj = reqPtr->process();
    //print resobj.data
    std::cout<< resObj.data<< std::endl;
    return 0;
}