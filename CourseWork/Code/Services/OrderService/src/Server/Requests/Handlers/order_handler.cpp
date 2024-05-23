#include"order_handler.h"

void OrderHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response)
{
    HTMLForm form(request, request.stream());
    try
    {
		std::string scheme;
        std::string info;
        long id {-1};
        std::string login;
        request.getCredentials(scheme, info);
        std::cout << "scheme: " << scheme << " identity: " << info << std::endl;
        if(scheme == "Bearer") 
		{
            if(!extractPayload(info,id,login)) 
			{
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_FORBIDDEN);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/not_authorized");
                root->set("title", "Internal exception");
                root->set("status", "403");
                root->set("detail", "User not authorized");
                root->set("instance", "/Order");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;                   
            }
        }
        std::cout << "id:" << id << " login:" << login << std::endl;
		if (request.getURI().find("/order")!=std::string::npos)
        {
			if(request.getURI().find("/create")!=std::string::npos)
			{
				if (form.has("id"), form.has("user_id") && form.has("accommodation_id") && form.has("accommodation_name"))
            	{
					std::vector<database::Order::Accommodation> accommodations;
					accommodations.push_back(database::Order::Accommodation(atol(form.get("accommodation_id").c_str()), form.get("accommodation_name")));
            	    database::Order order(atol(form.get("id").c_str()), atol(form.get("user_id").c_str()), accommodations);
            	    auto check_result = database::Accommodation::get(atol(form.get("accommodation_id").c_str()));
            	    std::string message;
            	    std::string reason;
            	    if (check_result)
            	    {
            	        order.insert();
            	        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            	        response.setChunkedTransferEncoding(true);
            	        response.setContentType("application/json");
            	        std::ostream &ostr = response.send();
            	        ostr << order.id;
            	        return;
            	    }
            	    else
            	    {
            	        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            	        std::ostream &ostr = response.send();
            	        ostr << message;
            	        response.send();
            	        return;
            	    }
            	}
			}
			else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT)
			{
				std::vector<database::Order::Accommodation> accommodations;
				for(int i=0; true; i++)
				{
					try
					{
						std::string string_array="accommodations["+std::to_string(i)+']';
						auto item_1 =form.get(string_array+"accommodation_id");
						auto item_2 =form.get(string_array+"accommodation_name");
						accommodations.push_back(database::Order::Accommodation(atol(item_1.c_str()), item_2));
					}
					catch(...)
					{
						break;
					}
				}
				database::Order order(atol(form.get("id").c_str()), atol(form.get("user_id").c_str()), accommodations);
            	order.modify();
            	response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            	response.setChunkedTransferEncoding(true);
            	response.setContentType("application/json");
            	std::ostream &ostr = response.send();
            	ostr << order.id;
            	return;
			}
			else
			{
				std::optional<database::Order> result;
				if(form.has("user_id"))
            		result = database::Order::getFromUserId(atol(form.get("user_id").c_str()));
            	if (result)
            	{
            	    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            	    response.setChunkedTransferEncoding(true);
            	    response.setContentType("application/json");
            	    std::ostream &ostr = response.send();
					Poco::JSON::Stringifier::stringify(result->toJSON(), ostr);
            	    return;
            	}
            	else
            	{
            	    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
            	    response.setChunkedTransferEncoding(true);
            	    response.setContentType("application/json");
            	    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            	    root->set("type", "/errors/not_found");
            	    root->set("title", "Internal exception");
            	    root->set("status", "404");
            	    root->set("detail", "Order not found");
            	    root->set("instance", "/order");
            	    std::ostream &ostr = response.send();
            	    Poco::JSON::Stringifier::stringify(root, ostr);
            	    return;
            	}
			}
		}
    }
    catch (Poco::Exception& e)
    {
		std::cerr << e.displayText() << std::endl;
    }
    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/not_found");
    root->set("title", "Internal exception");
    root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    root->set("detail", "request not found");
    root->set("instance", "/Order");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}