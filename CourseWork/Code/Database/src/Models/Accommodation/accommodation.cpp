#include <sstream>
#include <exception>

#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Dynamic/Var.h>

#include "accommodation.h"
#include "../../database.h"
#include "../../Config/config.h"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
	Poco::JSON::Object::Ptr Accommodation::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("_id", id);
        root->set("name", name);
        root->set("description", description);
		root->set("price", price);
        root->set("deadline", deadline);

        return root;
    }
	std::optional<Accommodation> Accommodation::get(long id)
    {
        std::optional<Accommodation> result;
        std::map<std::string,long> params; params["_id"] = id;
        auto results = database::Database::get().getFromMongo<long>("accommodations", params);

        if(!results.empty())
            result = Accommodation(results[0]);
        
        return result;
    }
	std::vector<Accommodation> Accommodation::getAll()
    {
        std::optional<Accommodation> result;
		std::map<std::string,long> params;
		auto results = database::Database::get().getFromMongo<long>("accommodations", params);
		std::vector<Accommodation> accommodations;

        if(!results.empty())
			for(auto result : results)
           		accommodations.push_back(Accommodation(result));
        
        return accommodations;
    }
	std::vector<Accommodation> Accommodation::searchByName(std::string name)
    {
        std::optional<Accommodation> result;
        std::map<std::string,std::string> params; params["name"] = name;
		auto results = database::Database::get().getFromMongo<std::string>("accommodations", params);
		std::vector<Accommodation> accommodations;

        if(!results.empty())
			for(auto result : results)
           		accommodations.push_back(Accommodation(result));
        
        return accommodations;
    }
    void Accommodation::insert()
    {
        database::Database::get().sendToMongo("accommodations", toJSON());
    }

    bool Accommodation::remove(long id)
    {
        std::map<std::string,long> params;
        params["_id"] = id;       
        return database::Database::get().deleteFromMongo("accommodations", params);
    }
}