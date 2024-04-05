#include <sstream>
#include <exception>

#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/PBKDF2Engine.h>
#include <Poco/HMACEngine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/DigestEngine.h>
#include <Poco/Random.h>

#include "user.h"
#include "../database.h"
#include "../../Config/config.h"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    void User::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();		//REVIEW
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
    }

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("login", _login);
        root->set("password", _password);

        return root;
    }
    User User::fromJSON(const std::string &str)
    {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        user.id() = object->getValue<long>("id");
        user.first_name() = object->getValue<std::string>("first_name");
        user.last_name() = object->getValue<std::string>("last_name");
        user.email() = object->getValue<std::string>("email");
        user.login() = object->getValue<std::string>("login");
        user.password() = object->getValue<std::string>("password");

        return user;
    }
    std::optional<long> User::authenticate(std::string &login, std::string &password)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            long id;

            select << "SELECT id FROM public.\"User\" WHERE login=$1 and password=$2",
                into(id),
                use(login), use(password),
                range(0, 1); //  iterate over result set one row at a time
            select.execute();

            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return id;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }
    std::optional<User> User::get(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            User user;

            select << "SELECT id, login, password, first_name, last_name, email FROM public.\"User\" WHERE id=$1",
				into(user._id), into(user._first_name), into(user._last_name), into(user._email), into(user._login), into(user._password),
            	use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return user;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }
    std::vector<User> User::getAll()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<User> result;
            User user;

            select << "SELECT id, login, password, first_name, last_name, email FROM users", 
				into(user._id), into(user._first_name), into(user._last_name), into(user._email), into(user._login), into(user._password),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(user);
            }
            return result;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }
	std::optional<User> User::searchByLogin(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<User> result;
            User user;

            select << "SELECT id, login, password, first_name, last_name, email FROM public.\"User\" WHERE login = $1",
                into(user._id), into(user._login), into(user._password), into(user._first_name), into(user._last_name), into(user._email),
                use(login),
                range(0, 1); //  iterate over result set one row at a time

			Poco::Data::RecordSet rs(select);
            if (rs.moveFirst())
                return user;
        }
        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
		return {};
    }
    std::vector<User> User::searchByName(std::string first_name, std::string last_name)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<User> result;
            User user;

            //first_name += "%";
            //last_name += "%";
            select << "SELECT id, login, password, first_name, last_name, email FROM public.\"User\" WHERE first_name LIKE $1 and last_name LIKE $2",
                into(user._id), into(user._login), into(user._password), into(user._first_name), into(user._last_name), into(user._email),
                use(first_name), use(last_name),
                range(0, 1); //  iterate over result set one row at a time
            while (!select.done())
                if (select.execute())
                    result.push_back(user);

            return result;
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }
    void User::insert()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session), select(session);
			Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> pbkdf2(std::to_string(Poco::Random().next()), 4096, 256);
			std::string password_hashed;
			long id;

			pbkdf2.update(_password);
			password_hashed=Poco::DigestEngine::digestToHex(pbkdf2.digest());

            insert << "INSERT INTO public.\"User\" (login, password, first_name, last_name, email) VALUES($1, $2, $3, $4, $5)", 
					use(_login), use(password_hashed), use(_first_name), use(_last_name), use(_email);
            insert.execute();

            select << "SELECT LASTVAL()",
                into(id),
                range(0, 1); //  iterate over result set one row at a time
            if (!select.done())
                select.execute();

            std::cout << "inserted:" << id << std::endl;
        }
        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }
	std::optional<User> User::modifyAdditionalInfo(long id, std::string first_name, std::string last_name, std::string email)	//TODO mofiyPassword
	{
		try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement update(session), select(session);

            update << "UPDATE public.\"User\" SET first_name=$2, last_name=$3, email=$4 WHERE id=$1", 
					use(id), use(first_name), use(last_name), use(email);
            update.execute();

            select << "SELECT LASTVAL()",
                into(id),
                range(0, 1); //  iterate over result set one row at a time
            if (!select.done())
                select.execute();

            std::cout << "updated:" << id << std::endl;
        }
        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
		return {};
	}
	void User::remove(long id)
	{
		try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement remove(session);
            User user;

            remove << "DELETE FROM public.\"User\" WHERE id=$1",
					use(id),
            remove.execute();
        }

        catch (Poco::Data::PostgreSQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::PostgreSQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
        }
	}

    const std::string &User::get_login() const
    {
        return _login;
    }
    const std::string &User::get_password() const
    {
        return _password;
    }
    std::string &User::login()
    {
        return _login;
    }
    std::string &User::password()
    {
        return _password;
    }
    long User::get_id() const
    {
        return _id;
    }
    const std::string &User::get_first_name() const
    {
        return _first_name;
    }
    const std::string &User::get_last_name() const
    {
        return _last_name;
    }
    const std::string &User::get_email() const
    {
        return _email;
    }
    long &User::id()
    {
        return _id;
    }
    std::string &User::first_name()
    {
        return _first_name;
    }
    std::string &User::last_name()
    {
        return _last_name;
    }
    std::string &User::email()
    {
        return _email;
    }
}