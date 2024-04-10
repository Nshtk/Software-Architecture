#ifndef Order_H
#define Order_H

#include <string>
#include <vector>
#include <optional>

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/DateTime.h>

namespace database
{
	class Order
	{
	private:

	public:
		struct Accommodation
		{
			long accommodation_id;
			std::string accommodation_name;

			Accommodation(long accommodation_id, std::string accommodation_name) 
						: accommodation_id(accommodation_id), accommodation_name(accommodation_name)
			{}
		};

		long id;
		long user_id;
		std::vector<Accommodation> accommodations;

		Order()
		{}
		Order(const long id, const long user_id, std::vector<Accommodation>& accommodations)
			: id(id), user_id(user_id), accommodations(accommodations)
		{}
		Order(const std::string& string_json)
		{
			Poco::JSON::Parser parser;
        	Poco::Dynamic::Var result = parser.parse(string_json);
       		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        	id = object->getValue<long>("_id");
        	user_id = object->getValue<long>("user_id");

			auto object_array = object->getArray("accommodations");
			for (Poco::JSON::Array::ConstIterator it = object_array->begin(); it != object_array->end(); ++it)
			{
				Poco::Dynamic::Var result = parser.parse(it->convert<std::string>());
       			Poco::JSON::Object::Ptr object_nested = result.extract<Poco::JSON::Object::Ptr>();
				accommodations.push_back(Accommodation(object_nested->getValue<long>("accommodation_id"), object_nested->getValue<std::string>("accommodation_name")));
			}
		}

		static std::optional<Order> get(long id);
		static std::optional<Order> getFromUserId(long id);
		static std::vector<Order> searchByName(std::string name);
		static bool remove(long id);

        void insert();
		void modify();
        Poco::JSON::Object::Ptr toJSON() const;
	};
}

#endif