#ifndef USEHANDLER_H
#define USEHANDLER_H

#include <iostream>
#include <iostream>
#include <fstream>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include "../../../../../../Database/src/Models/User/user.h"
#include "../../../../../../Common/src/Utility/utility.h"

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

class UserHandler : public HTTPRequestHandler
{
private:
    bool validateName(const std::string &name, std::string &reason)
    {
        if (name.length() < 3)
        {
            reason = "Name must be at least 3 signs";
            return false;
        }
        if (name.find(' ') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }
        if (name.find('\t') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        return true;
    };

    bool check_email(const std::string &email, std::string &reason)
    {
        if (email.find('@') == std::string::npos)
        {
            reason = "Email must contain @";
            return false;
        }

        if (email.find(' ') != std::string::npos)
        {
            reason = "EMail can't contain spaces";
            return false;
        }

        if (email.find('\t') != std::string::npos)
        {
            reason = "EMail can't contain spaces";
            return false;
        }

        return true;
    };

public:
    UserHandler(const std::string &format) : _format(format)
    {}

    Poco::JSON::Object::Ptr remove_password(Poco::JSON::Object::Ptr src)
    {
        if (src->has("password"))
            src->set("password", "*******");
        return src;
    }
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());

        try
        {
            if (request.getURI().find("/user")!=std::string::npos && request.getMethod()==Poco::Net::HTTPRequest::HTTP_GET)
            {
				std::optional<database::User> result;
				std::vector<database::User> result_all;

				if(form.has("id"))
                	result = database::User::get(atol(form.get("id").c_str()));
				else
					result_all=database::User::getAll();

				int result_all_size=result_all.size();
                if (result || result_all_size)
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
					if(result)
						Poco::JSON::Stringifier::stringify(remove_password(result->toJSON()), ostr);
					else
						for(int i=0; i<result_all_size; i++)
							Poco::JSON::Stringifier::stringify(remove_password(result_all[i].toJSON()), ostr);
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
                    root->set("detail", "user not found");
                    root->set("instance", "/user");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }
            }
            else if (request.getURI().find("/auth")!=std::string::npos)
            {
                std::string scheme;
                std::string info;
                request.getCredentials(scheme, info);
                std::cout << "scheme: " << scheme << " identity: " << info << std::endl;

                std::string login, password;
                if (scheme == "Basic")
                {
                    getIdentity(info, login, password);
                    if (auto id = database::User::authenticate(login, password))
                    {
                        std::string token = generateToken(*id,login);
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        ostr << "{ \"id\" : \"" << *id << "\", \"login\" : \"" << login << "\", \"Token\" : \""<< token <<"\"}" << std::endl;
                        return;
                    }
                }

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
            else if (request.getURI().find("/search")!=std::string::npos)
            {
				std::string login = form.get("login");
                std::string fn = form.get("first_name");
                std::string ln = form.get("last_name");
				Poco::JSON::Array arr;

				if(!login.empty())
				{
					auto result=database::User::searchByLogin(login);
					if(!result)
					{
						response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                        std::ostream &ostr = response.send();
                        ostr << "User not found";
                        response.send();
                        return;
					}
					arr.add(remove_password(result.value().toJSON()));
				}
				else
				{
					auto results = database::User::searchByName(fn, ln);
					if(results.empty())
					{
						response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                        std::ostream &ostr = response.send();
                        ostr << "Users not found";
                        response.send();
                        return;
					}
                	for (auto s : results)
                    	arr.add(remove_password(s.toJSON()));
				}
				
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(arr, ostr);

                return;
            }
            else if (request.getURI().find("/register")!=std::string::npos)
            {
                if (form.has("first_name") && form.has("last_name") && form.has("email") && form.has("login") && form.has("password"))
                {
                    database::User user(form.get("login"), form.get("password"), form.get("first_name"), form.get("last_name"), form.get("email"));
                    bool check_result = true;
                    std::string message;
                    std::string reason;

                    if (!validateName(user.first_name, reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }
                    if (!validateName(user.last_name, reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }
                    if (!check_email(user.email, reason))
                    {
                        check_result = false;
                        message += reason;
                        message += "<br>";
                    }
                    if (check_result)
                    {
                        user.insert();
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        ostr << user.id;
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
        root->set("instance", "/user");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

private:
    std::string _format;
};
#endif