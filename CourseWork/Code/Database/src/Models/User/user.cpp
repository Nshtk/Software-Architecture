#include <sstream>
#include <exception>

#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/PBKDF2Engine.h>
#include <Poco/HMACEngine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/DigestEngine.h>
#include <Poco/Random.h>

#include "user.h"
#include "../../database.h"
#include "../../Config/config.h"
#include "../../Cache/cache.h"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", id);
        root->set("first_name", first_name);
        root->set("last_name", last_name);
        root->set("email", email);
        root->set("login", login);
        root->set("password", password);

        return root;
    }
    std::optional<long> User::authenticate(std::string &login, std::string &password)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement select(session);
            long id;

            select << "SELECT id FROM public.\"User\" WHERE login=$1 and password=$2",
                into(id),
                use(login), use(password),
                range(0, 1);  //Iterate over result set one row at a time
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
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement select(session);
            User user;

            select << "SELECT id, login, password, first_name, last_name, email FROM public.\"User\" WHERE id=$1",
				into(user.id), into(user.first_name), into(user.last_name), into(user.email), into(user.login), into(user.password),
            	use(id),
                range(0, 1);

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
	 std::optional<User> User::getWithCache(long id)
    {
        try
        {
            std::string result=database::Cache::Instance.get(std::to_string(id));
            if (!result.empty())
                return User(result);
            else
                return std::optional<User>();
        }
        catch (std::exception& err)
        {
            return std::optional<User>();
        }
    }
    std::vector<User> User::getAll()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().createSession();
            Statement select(session);
            std::vector<User> result;
            User user;

            select << "SELECT id, login, password, first_name, last_name, email FROM public.\"User\"", 
				into(user.id), into(user.first_name), into(user.last_name), into(user.email), into(user.login), into(user.password),
                range(0, 1); 

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
            Poco::Data::Session session = database::Database::get().createSession();
            Statement select(session);
            std::vector<User> result;
            User user;

            select << "SELECT id, login, password, first_name, last_name, email FROM public.\"User\" WHERE login = $1",
                into(user.id), into(user.login), into(user.password), into(user.first_name), into(user.last_name), into(user.email),
                use(login),
                range(0, 1);

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
            Poco::Data::Session session = database::Database::get().createSession();
            Statement select(session);
            std::vector<User> result;
            User user;

            //first_name += "%";
            //last_name += "%";
            select << "SELECT id, login, password, first_name, last_name, email FROM public.\"User\" WHERE first_name LIKE $1 and last_name LIKE $2",
                into(user.id), into(user.login), into(user.password), into(user.first_name), into(user.last_name), into(user.email),
                use(first_name), use(last_name),
                range(0, 1);
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
            Poco::Data::Session session = database::Database::get().createSession();
            Poco::Data::Statement insert(session), select(session);
			Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> pbkdf2(std::to_string(Poco::Random().next()), 4096, 256);
			std::string password_hashed;
			long id;

			pbkdf2.update(password);
			password_hashed=Poco::DigestEngine::digestToHex(pbkdf2.digest());

            insert << "INSERT INTO public.\"User\" (login, password, first_name, last_name, email) VALUES($1, $2, $3, $4, $5)", 
					use(login), use(password_hashed), use(first_name), use(last_name), use(email);
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
	void User::insertWithCache()
    {
        std::stringstream string_stream;
        Poco::JSON::Stringifier::stringify(toJSON(), string_stream);
        std::string message = string_stream.str();
        database::Cache::Instance.insert(std::to_string(id), message, "60");
    }
	std::optional<User> User::modifyAdditionalInfo(long id, std::string first_name, std::string last_name, std::string email)	//TODO mofiyPassword()
	{
		try
        {
            Poco::Data::Session session = database::Database::get().createSession();
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
            Poco::Data::Session session = database::Database::get().createSession();
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
}