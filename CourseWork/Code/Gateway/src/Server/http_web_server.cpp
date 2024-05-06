#include "http_web_server.h"

int HTTPWebServer::main(const std::vector<std::string> &args)
{
	int port=8081;
	ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", port));
    HTTPServer srv(new HTTPRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
    std::cout << "Started gateway on port " << port << std::endl;
    srv.start();
    waitForTerminationRequest();
    srv.stop();
    return Application::EXIT_OK;
}