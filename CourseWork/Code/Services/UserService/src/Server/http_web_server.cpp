#include "http_web_server.h"

int HTTPWebServer::main(const std::vector<std::string> &args)
{
	try
    {
        Poco::Data::Session session = database::Database::get().createSession();
    }
    catch (Poco::Data::PostgreSQL::PostgreSQLException &e)
    {
        std::cout << "connection:" << e.displayText() << std::endl;
        throw;
    }
    catch (Poco::Data::ConnectionFailedException &e)
    {
        std::cout << "connection:" << e.displayText() << std::endl;
        throw;
    }
	ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", 8081));
	HTTPServer srv(new HTTPRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
	srv.start();
	waitForTerminationRequest();
	srv.stop();
    return Application::EXIT_OK;
}