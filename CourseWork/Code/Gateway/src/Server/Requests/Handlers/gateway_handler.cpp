#include "gateway_handler.h"

inline std::string GatewayHandler::getKey(const std::string &method, const std::string &base_path, const std::string &query, const std::string &basic_auth)
{
    return method+":"+base_path+":"+query+":"+basic_auth;
}
std::string GatewayHandler::getFromCache(const std::string &method, const std::string &base_path, const std::string &query, const std::string &basic_auth)
{
    std::string key = getKey(method, base_path, query, basic_auth);
    std::string result;
    if(database::Cache::getInstance().get(key, result))
        return result;
	else
		return std::string();
}
void GatewayHandler::insertToCache(const std::string &method, const std::string &base_path, const std::string &query, const std::string &basic_auth,const std::string &result)
{
    std::string key = getKey(method, base_path, query, basic_auth);
    database::Cache::getInstance().put(key,result);
}
std::string GatewayHandler::sendRequest(const std::string &method, const std::string &base_path, const std::string &query, const std::string &basic_auth, const std::string &token, const std::string &body)
{
    try
    {
        Poco::URI uri(base_path + query);
        std::string path(uri.getPathAndQuery());
		Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
		Poco::Net::HTTPRequest request(method, path, Poco::Net::HTTPMessage::HTTP_1_1);
		
        if (path.empty())
            path = "/";

        request.setContentType("application/json");
        request.setContentLength(body.length());
        if (!basic_auth.empty())
            request.set("Authorization", "Basic " + basic_auth);
        else if (!token.empty())
			request.set("Authorization", "Bearer " + token);
        session.sendRequest(request) << body;
        Poco::Net::HTTPResponse response;
        std::istream &rs = session.receiveResponse(response);
        std::stringstream string_stream;
        Poco::StreamCopier::copyStream(rs, string_stream);
		
        return string_stream.str();
    }
    catch (Poco::Exception &ex)
    {
        std::cerr << ex.displayText() << std::endl;
        return std::string();
    }
    return std::string();
}
void GatewayHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response)
{
    std::string base_url_user = std::getenv("USER_ADDRESS"), base_url_accommodation = std::getenv("ACCOMMODATION_ADDRESS"), base_url_order = std::getenv("ORDER_ADDRESS");
    std::string scheme, info;
	std::string login, password;
	
    request.getCredentials(scheme, info);
    std::cout << "Gateway - scheme: " << scheme << " identity: " << info << std::endl;
    if (scheme == "Basic")
    {
        if(request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) 
		{
            std::string cache_result = getFromCache(request.getMethod(), base_url_user, request.getURI(), info);
            if(!cache_result.empty())
			{
                std::cout << "Gateway - from cache: " << cache_result << std::endl;
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                ostr << cache_result;
                ostr.flush();
                return;
            }
        }
        std::string token = sendRequest(Poco::Net::HTTPRequest::HTTP_GET, base_url_user, "/auth", info, "","");
        std::cout << "Gateway - auth:" << token << std::endl;
        if (!token.empty())
        {
            std::string real_result = sendRequest(request.getMethod(), base_url_accommodation, request.getURI(), "", token, "");
            std::cout << "Gateway - result: " << std::endl << real_result << std::endl;
            if(!real_result.empty() || real_result.find("error")!=std::string::npos)
			{
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                ostr << real_result;
                ostr.flush();
                insertToCache(request.getMethod(), base_url_user, request.getURI(),info,real_result);
            } 
			else 
			{
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/unauthorized");
                root->set("title", "Internal exception");
                root->set("status", "401");
                root->set("detail", "not authorized");
                root->set("instance", "/auth");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
            }
        }
    }
}