#include"http_request_factory.h"

HTTPRequestHandler* HTTPRequestFactory::createRequestHandler(const HTTPServerRequest& request)
{
    std::cout << "request:" << request.getURI()<< std::endl;
    if (request.getURI().find("/accommodation")!=std::string::npos) 
        return new AccommodationHandler(_format);
    return 0;
}