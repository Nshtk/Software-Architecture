#include <sstream>
#include <exception>

#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Dynamic/Var.h>

#include "order.h"
#include "../../database.h"
#include "../../Config/config.h"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
	Poco::JSON::Object::Ptr Order::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("_id", id);
        root->set("user_id", user_id);
		Poco::JSON::Array object_array;

		for(int i=0; i<accommodations.size(); i++)
		{
			Poco::JSON::Object::Ptr obj;
			obj->set("accommodation_id", accommodations[i].accommodation_id);
			obj->set("accommodation_name", accommodations[i].accommodation_name);
			object_array.add(obj);
		}
		root->set("accommodations", object_array);

        return root;
    }
	std::optional<Order> Order::get(long id)
    {
        std::optional<Order> result;
        std::map<std::string,long> params; params["_id"] = id;
        auto results = database::Database::get().getFromMongo<long>("Orders", params);

        if(!results.empty())
            result = Order(results[0]);
        
        return result;
    }

	void Order::modify()
    {
        std::map<std::string,long> params;
        params["_id"] = id;       
        database::Database::get().updateInMongo("orders",params,toJSON());
    }
    bool Order::remove(long id)
    {
        std::map<std::string,long> params;
        params["_id"] = id;       
        return database::Database::get().deleteFromMongo("Orders", params);
    }
}