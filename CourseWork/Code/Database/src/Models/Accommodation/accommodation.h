#ifndef ACCOMMODATION_H
#define ACCOMMODATION_H

#include <string>
#include <vector>
#include <optional>

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/DateTime.h>

namespace database
{
	class Accommodation
	{
	private:

	public:
		long id;
		std::string name;
		std::string description;
		int price;
		Poco::DateTime deadline;


		Accommodation()
		{}
		Accommodation(const long id, const std::string& name, const std::string& description, const int price, const Poco::DateTime& now)
			: id(id), name(name), description(description), price(price), deadline(now)
		{}
		Accommodation(const std::string& string_json)
		{
			Poco::JSON::Parser parser;
        	Poco::Dynamic::Var result = parser.parse(string_json);
       		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        	id = object->getValue<long>("_id");
        	name = object->getValue<std::string>("name");
       		description = object->getValue<std::string>("description");
        	price = object->getValue<int>("price"); 
        	deadline = object->getValue<Poco::DateTime>("deadline");
		}

		static std::optional<Accommodation> get(long id);
		static std::vector<Accommodation> getAll();
		static std::vector<Accommodation> searchByName(std::string name);
		static bool remove(long id);

        void insert();
        Poco::JSON::Object::Ptr toJSON() const;
	};
}

#endif