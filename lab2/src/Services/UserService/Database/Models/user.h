#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <optional>

#include "Poco/JSON/Object.h"

namespace database
{
    class User
	{
        private:
            long _id;
            std::string _first_name;
            std::string _last_name;
            std::string _email;
            std::string _login;
            std::string _password;

        public:
            static User fromJSON(const std::string&  str);
			static void init();

            long get_id() const;
			long& id();
            const std::string& get_first_name() const;
            const std::string& get_last_name() const;
            const std::string& get_email() const;
            const std::string& get_login() const;
            const std::string& get_password() const;
            std::string& first_name();
            std::string& last_name();
            std::string& email();
            std::string& login();
            std::string& password();

            static std::optional<User> get(long id);
			static std::optional<User> modifyAdditionalInfo(long id, std::string first_name, std::string last_name, std::string email);
			static void remove(long id);
            static std::optional<long> authenticate(std::string& login, std::string& password);
            static std::vector<User> getAll();
			static std::optional<User> searchByLogin(std::string login);
            static std::vector<User> searchByName(std::string first_name,std::string last_name);
            void insert();


            Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif