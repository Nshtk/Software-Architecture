#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <optional>

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>

namespace database
{
    class User
	{
        private:
            
        public:
			long id;
			std::string login;
            std::string password;
            std::string first_name;
            std::string last_name;
            std::string email;

			User()
			{}
			User(const std::string& login, const std::string& password, const std::string& first_name, const std::string& last_name, const std::string& email)
				: login(login), password(password), first_name(first_name), last_name(last_name), email(email)
			{}
			User(const std::string& string_json)	//From JSON
			{
        		Poco::JSON::Parser parser;
        		Poco::Dynamic::Var result = parser.parse(string_json);
       			Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        		id = object->getValue<long>("id");
        		login = object->getValue<std::string>("login");
       		 	password = object->getValue<std::string>("password");
        		first_name = object->getValue<std::string>("first_name"); 
        		last_name = object->getValue<std::string>("last_name");
        		email = object->getValue<std::string>("email"); 
			}

            static std::optional<User> get(long id);
			static std::optional<User> getWithCache(long id);
			static std::optional<User> modifyAdditionalInfo(long id, std::string first_name, std::string last_name, std::string email);
			static void remove(long id);
            static std::optional<long> authenticate(std::string& login, std::string& password);
            static std::vector<User> getAll();
			static std::optional<User> searchByLogin(std::string login);
            static std::vector<User> searchByName(std::string first_name,std::string last_name);
            
			void insert();
			void insertWithCache();
            Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif