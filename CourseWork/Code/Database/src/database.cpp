#include <stdexcept>

#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include "Config/config.h"
#include "database.h"

namespace database
{
    Database::Database() : database_mongo(Config::get().getMongoDatabase())
    {
		_connection_string.append("host=").append(Config::get().get_host()).append(" user=").append(Config::get().get_login()).append(" dbname=").append(Config::get().get_database()).append(" port=").append(Config::get().get_port()).append(" password=").append(Config::get().get_password());

        std::cout << "# Connecting to postgresql: " << _connection_string << std::endl;
        Poco::Data::PostgreSQL::Connector::registerConnector();
        _pool = std::make_unique<Poco::Data::SessionPool>(Poco::Data::PostgreSQL::Connector::KEY, _connection_string);

        std::cout << "# Connecting to mongodb: " << Config::get().get_mongo() << ":" << Config::get().get_mongo_port()  << std::endl;
        connection_mongo.connect(Config::get().get_mongo(), atoi(Config::get().get_mongo_port().c_str()));
    }

    Poco::MongoDB::Database &Database::getMongoDatabase()
    {
        return database_mongo;
    }
    Database& Database::get()
    {
        static Database _instance;
        return _instance;
    }
    Poco::Data::Session Database::createSession()
    {
        return Poco::Data::Session(_pool->get());
    }
    long Database::countFromMongo(const std::string &collection, std::map<std::string, long> &params)
    {
        try
        {
            Poco::SharedPtr<Poco::MongoDB::QueryRequest> request = database_mongo.createCountRequest(collection);
			Poco::MongoDB::ResponseMessage response;

            auto &query = request->selector().addNewDocument("query");
            for (auto &[key, val] : params)
                query.addNewDocument(key).add("$eq",val);
            connection_mongo.sendRequest(*request, response);

            if ( response.hasDocuments() )
                return response.documents()[0]->getInteger("n");
        }
        catch (std::exception &ex)
        {
            std::cout << "mongodb exception: " << ex.what() << std::endl;
            std::string lastError = database_mongo.getLastError(connection_mongo);
            if (!lastError.empty())
                std::cout << "mongodb Last Error: " << lastError << std::endl;
        }

        return -1;
    }
    void Database::updateInMongo(const std::string &collection, std::map<std::string, long> &params, Poco::JSON::Object::Ptr json)
    {
        try
        {
            Poco::SharedPtr<Poco::MongoDB::UpdateRequest> request = database_mongo.createUpdateRequest(collection);
            for (auto &[key, val] : params)
                request->selector().add(key, val);
            std::function<void(Poco::MongoDB::Document &, Poco::JSON::Object::Ptr &)> fill_document;

            fill_document = [&](Poco::MongoDB::Document &doc, Poco::JSON::Object::Ptr &obj) -> void
            {
                for (auto &val : *obj)
                {
                    if (val.second.isInteger())
                    {
                        int v;
                        val.second.convert(v);
                        doc.add(val.first, v);
                    }
                    else if (val.second.isString())
                        doc.add(val.first, val.second.extract<std::string>());
                    else
                    {
                        try
                        {
                            Poco::JSON::Object::Ptr child = val.second.extract<Poco::JSON::Object::Ptr>();
                            Poco::MongoDB::Document &child_doc = doc.addNewDocument(val.first);
                            fill_document(child_doc, child);
                        }
                        catch (...)
                        {
                            doc.add(val.first, val.second.toString());
                        }
                    }
                }
            };

            Poco::MongoDB::Document &doc = request->update();
            fill_document(doc, json);
            connection_mongo.sendRequest(*request);
        }
        catch (std::exception &ex)
        {
            std::cout << "mongodb exception: " << ex.what() << std::endl;
            std::string lastError = database_mongo.getLastError(connection_mongo);
            if (!lastError.empty())
                std::cout << "mongodb Last Error: " << lastError << std::endl;
        }
    }
	bool Database::deleteFromMongo(const std::string &collection, std::map<std::string, long> &params)
	{
		try
        {
            Poco::SharedPtr<Poco::MongoDB::DeleteRequest> request = database_mongo.createDeleteRequest(collection);
            for (auto &[key, val] : params)
                request->selector().add(key, val);
			connection_mongo.sendRequest(*request);
			return true;
        }
        catch (std::exception &ex)
        {
            std::cout << "mongodb exception: " << ex.what() << std::endl;
            std::string lastError = database_mongo.getLastError(connection_mongo);
            if (!lastError.empty())
                std::cout << "mongodb Last Error: " << lastError << std::endl;
			return false;
        }
	}
    void Database::sendToMongo(const std::string &collection, Poco::JSON::Object::Ptr json)
    {
        try
        {
            Poco::SharedPtr<Poco::MongoDB::InsertRequest> insertRequest = database_mongo.createInsertRequest(collection);
            std::function<void(Poco::MongoDB::Document &, Poco::JSON::Object::Ptr &)> fill_document = [&](Poco::MongoDB::Document &doc, Poco::JSON::Object::Ptr &obj) -> void
            {
                for (auto &val : *obj)
                {
                    if (val.second.isInteger())
                    {
                        int v;
                        val.second.convert(v);
                        doc.add(val.first, v);
                    }
                    else if (val.second.isString())
                    {
                        doc.add(val.first, val.second.extract<std::string>());
                    }
                    else
                    {
                        try
                        {
                            Poco::JSON::Object::Ptr child = val.second.extract<Poco::JSON::Object::Ptr>();
                            Poco::MongoDB::Document &child_doc = doc.addNewDocument(val.first);
                            fill_document(child_doc, child);
                        }
                        catch (...)
                        {
                            doc.add(val.first, val.second.toString());
                        }
                    }
                }
            };

            Poco::MongoDB::Document &doc = insertRequest->addNewDocument();
            fill_document(doc, json);
            connection_mongo.sendRequest(*insertRequest);
        }
        catch (std::exception &ex)
        {
            std::cout << "mongodb exception: " << ex.what() << std::endl;
            std::string lastError = database_mongo.getLastError(connection_mongo);
            if (!lastError.empty())
                std::cout << "mongodb Last Error: " << lastError << std::endl;
        }
    }
}