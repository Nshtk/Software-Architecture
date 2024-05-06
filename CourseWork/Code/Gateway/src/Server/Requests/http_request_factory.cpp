#include"http_request_factory.h"

HTTPRequestHandler* HTTPRequestFactory::createRequestHandler(const HTTPServerRequest& request)
{
    return new GatewayHandler();
}