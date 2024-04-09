#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <memory>
#include <vector>
#include <map>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>

#include <Poco/Data/PostgreSQL/Connector.h>
#include <Poco/Data/PostgreSQL/PostgreSQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/SessionPool.h>

#include <Poco/MongoDB/MongoDB.h>
#include <Poco/MongoDB/Connection.h>
#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/Cursor.h>

namespace database
{
    class Database{
        private:
            std::string _connection_string;
            std::unique_ptr<Poco::Data::SessionPool> _pool;

            Poco::MongoDB::Connection connection_mongo;
            Poco::MongoDB::Database   database_mongo;

            Database();
        public:
            static Database& get();
            Poco::Data::Session createSession();
            Poco::MongoDB::Database& getMongoDatabase();
            void sendToMongo(const std::string& collection, Poco::JSON::Object::Ptr json);
            void updateInMongo(const std::string& collection,std::map<std::string,long>& params, Poco::JSON::Object::Ptr json);
            //template<typename T> std::vector<std::string> getFromMongo(const std::string& collection, std::map<std::string,T>& params);
            long countFromMongo(const std::string& collection, std::map<std::string,long>& params);
			bool deleteFromMongo(const std::string &collection, std::map<std::string, long> &params);
			template<typename T> std::vector<std::string> getFromMongo(const std::string& collection, std::map<std::string,T>& params)
    		{
    		   std::vector<std::string> result;
    		    try
    		    {
    		        Poco::MongoDB::QueryRequest request("sa-accommodation-service-mongo."+collection);
    		        Poco::MongoDB::ResponseMessage response;

    		        for (auto &[key, val] : params)
    		            request.selector().add(key, val);
    		        connection_mongo.sendRequest(request, response);

    		        for (auto doc : response.documents())
    		            result.push_back(doc->toString());
    		    }
    		    catch (std::exception &ex)
    		    {
    		        std::cout << "mongodb exception: " << ex.what() << std::endl;
    		        std::string lastError = database_mongo.getLastError(connection_mongo);
    		        if (!lastError.empty())
    		            std::cout << "mongodb Last Error: " << lastError << std::endl;
    		    }
    		    return result;
    		}
	};
}
#endif