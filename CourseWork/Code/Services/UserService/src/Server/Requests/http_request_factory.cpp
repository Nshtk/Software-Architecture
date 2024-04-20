#include "http_request_factory.h"

HTTPRequestHandler* HTTPRequestFactory::createRequestHandler(const HTTPServerRequest& request)
{
    std::cout << "request:" << request.getURI()<< std::endl;
    if (request.getURI().find("/user") || request.getURI().find("/search") || request.getURI().find("/auth")) 
        return new UserHandler(_format);
    return 0;
}