#include"accommodation_handler.h"

void AccommodationHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response)
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
            if(!extractPayload(info, id, login)) 
			{
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_FORBIDDEN);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/not_authorized");
                root->set("title", "Internal exception");
                root->set("status", "403");
                root->set("detail", "Accommodation not authorized");
                root->set("instance", "/accommodation");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;                   
            }
        }
        std::cout << "id:" << id << " login:" << login << std::endl;
		if (request.getURI().find("/accommodation")!=std::string::npos)
        {
			if(request.getURI().find("/search")!=std::string::npos)
			{
				std::string name = form.get("name");
				Poco::JSON::Array arr;
				if(!login.empty())
				{
					auto results = database::Accommodation::searchByName(name);
					if(results.empty())
					{
						response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                	    std::ostream &ostr = response.send();
                	    ostr << "Accommodations not found";
                	    response.send();
                	    return;
					}
            		for (auto s : results)
                		arr.add(s.toJSON());
				}
				else
				{
					auto results = database::Accommodation::searchByName(name);
					if(results.empty())
					{
						response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    	std::ostream &ostr = response.send();
                    	ostr << "Accommodations not found";
                    	response.send();
                    	return;
					}
            		for (auto s : results)
                		arr.add(s.toJSON());
				}
			
            	response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            	response.setChunkedTransferEncoding(true);
            	response.setContentType("application/json");
            	std::ostream &ostr = response.send();
            	Poco::JSON::Stringifier::stringify(arr, ostr);
            	return;
			}
			else if(request.getMethod()==Poco::Net::HTTPRequest::HTTP_DELETE)
			{
				bool result;
				if(form.has("id"))
            		result = database::Accommodation::remove(atol(form.get("id").c_str()));
            	if (result)
            	{
            	    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            	    response.setChunkedTransferEncoding(true);
            	    response.setContentType("application/json");
            	    std::ostream &ostr = response.send();
					if(result)
						Poco::JSON::Stringifier::stringify(id, ostr);
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
            	    root->set("detail", "Accommodation not deleted");
            	    root->set("instance", "/Accommodation");
            	    std::ostream &ostr = response.send();
            	    Poco::JSON::Stringifier::stringify(root, ostr);
            	    return;
            	}
			}
			else if(request.getURI().find("/create")!=std::string::npos && request.getMethod()==Poco::Net::HTTPRequest::HTTP_POST)
			{
				if (form.has("id"), form.has("name") && form.has("description") && form.has("price") && form.has("deadline"))
            	{
					int tz=1;
            	    database::Accommodation Accommodation(atol(form.get("id").c_str()), form.get("name"), form.get("description"), atoi(form.get("price").c_str()), Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, form.get("deadline"), tz));
            	    bool check_result = true;
            	    std::string message;
            	    std::string reason;
            	    if (check_result)
            	    {
            	        Accommodation.insert();
            	        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            	        response.setChunkedTransferEncoding(true);
            	        response.setContentType("application/json");
            	        std::ostream &ostr = response.send();
            	        ostr << Accommodation.id;
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
			else
			{
				std::optional<database::Accommodation> result;
				std::vector<database::Accommodation> result_all;
				if(form.has("id"))
            		result = database::Accommodation::get(atol(form.get("id").c_str()));
				else
					result_all=database::Accommodation::getAll();
				int result_all_size=result_all.size();
            	if (result || result_all_size)
            	{
            	    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            	    response.setChunkedTransferEncoding(true);
            	    response.setContentType("application/json");
            	    std::ostream &ostr = response.send();
					if(result)
						Poco::JSON::Stringifier::stringify(result->toJSON(), ostr);
					else
						for(int i=0; i<result_all_size; i++)
							Poco::JSON::Stringifier::stringify(result_all[i].toJSON(), ostr);
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
            	    root->set("detail", "Accommodation not found");
            	    root->set("instance", "/Accommodation");
            	    std::ostream &ostr = response.send();
            	    Poco::JSON::Stringifier::stringify(root, ostr);
            	    return;
            	}
			}
		}
		else
		{
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
    root->set("instance", "/accommodation");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}