#include "gateway_handler.h"
#include "Poco/Net/HTMLForm.h"

inline std::string GatewayHandler::getKey(const std::string &method, const std::string &base_path, const std::string &query, const std::string &auth)
{
    return method+":"+base_path+":"+query+":"+auth;
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
void GatewayHandler::getResponseFromService(const std::string& service_name, HTTPServerRequest& request, HTTPServerResponse& response, std::string& base_url, std::string& auth_info)
{
	std::string cache_result = database::Cache::Instance.get(getKey(request.getMethod(), base_url, request.getURI(), auth_info));
    if(!cache_result.empty())
	{
        std::cout << "Gateway - from cache: " << cache_result << std::endl;
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        ostr << cache_result;
        ostr.flush();
    }
	else if(CircuitBreaker::Instance.isServiceAvailable(service_name))
	{
		std::string body="";
		if(request.getMethod()!=Poco::Net::HTTPRequest::HTTP_GET)
        	Poco::StreamCopier::copyToString(request.stream(), body);
			
		std::string real_result = sendRequest(request.getMethod(), base_url, request.getURI(), "", auth_info, body);
		std::cout << "Gateway - from request: " << std::endl << real_result << std::endl;
    	if(!real_result.empty() && real_result.find("error")==std::string::npos)
		{
    	    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    	    response.setChunkedTransferEncoding(true);
    	    response.setContentType("application/json");
    	    std::ostream &ostr = response.send();
    	    ostr << real_result;
    	    ostr.flush();
			
			//database::Cache::Instance.insert(getKey(request.getMethod(), base_url, request.getURI(), auth_info), real_result, "60");
			CircuitBreaker::Instance.success(service_name);
    	}
		else
		{
			response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE);
    	    response.setChunkedTransferEncoding(true);
    	    response.setContentType("application/json");
    	    std::ostream &ostr = response.send();
    	    ostr << real_result;
    	    ostr.flush();
		 	CircuitBreaker::Instance.fail(service_name);
		}
	}
	else
	{
		response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/unavailable");
        root->set("title", "Service unavailable");
        root->set("status", "503");
        root->set("detail", "circuit breaker open");
        root->set("instance", request.getURI());
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);  
	}
}
void GatewayHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::string base_url_user = std::getenv("USER_ADDRESS"), base_url_accommodation = std::getenv("ACCOMMODATION_ADDRESS"), base_url_order = std::getenv("ORDER_ADDRESS");
    std::string token;
	std::string scheme, auth_info;
	
	request.getCredentials(scheme, auth_info);	
	std::cout << "Gateway - scheme: " << scheme << " identity: " << auth_info << std::endl;
	
	if (request.getURI().find("/auth")!=std::string::npos)
    {
		std::string login, password;
		
		token = sendRequest(Poco::Net::HTTPRequest::HTTP_GET, base_url_user, "/auth", auth_info, "", "");
		response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    	response.setChunkedTransferEncoding(true);
    	response.setContentType("application/json");
    	std::ostream &ostr = response.send();
    	ostr << token;
    	ostr.flush();
		
		std::cout << "Gateway - token:" << token << std::endl;
		database::Cache::Instance.insert(auth_info, token, "999");
    }
	else
	{
		if (scheme == "Basic")
		{
			token = database::Cache::Instance.get(auth_info);
    		if(token.empty())
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
				return;
    		}
		}
		if(request.getURI().find("/user")!=std::string::npos)
		{
			getResponseFromService("user_service", request, response, base_url_user, token);
		}
		else if(request.getURI().find("/accommodation")!=std::string::npos)
		{
			getResponseFromService("accommodation_service", request, response, base_url_accommodation, token);
		}
		else if(request.getURI().find("/order")!=std::string::npos)
		{
			getResponseFromService("order_service", request, response, base_url_order, token);
		}
	}
}