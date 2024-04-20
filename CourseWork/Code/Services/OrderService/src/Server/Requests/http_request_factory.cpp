#include"http_request_factory.h"

HTTPRequestHandler* HTTPRequestFactory::createRequestHandler(const HTTPServerRequest& request)
{
    std::cout << "request:" << request.getURI()<< std::endl;
    if (request.getURI().find("/order")!=std::string::npos) 
        return new OrderHandler(_format);
    return 0;
}