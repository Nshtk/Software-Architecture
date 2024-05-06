#include "cache.h"
#include "../Config/config.h"

#include <exception>
#include <mutex>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

namespace database
{
    std::mutex _mtx;
	
    Cache::Cache()
    {
        std::string host = Config::get().getCacheServers().substr(0, Config::get().getCacheServers().find(':'));
        std::string port = Config::get().getCacheServers().substr(Config::get().getCacheServers().find(':') + 1);

        std::cout << "cache host:" << host <<" port:" << port << std::endl;
        _stream = rediscpp::make_stream(host, port);
    }

    Cache Cache::getInstance()
    {
        static Cache instance;
        return instance;
    }
    void Cache::put(const std::string& id, const std::string& val)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        rediscpp::value response = rediscpp::execute(*_stream, "set", id, val, "ex", "60");		//Lifetime 60?
    }
    bool Cache::get(const std::string& id, std::string& val)
    {
        try
		{
        	std::lock_guard<std::mutex> lck(_mtx);
        	rediscpp::value response = rediscpp::execute(*_stream, "get", id);
	
        	if (response.empty() || response.is_error_message())
        	    return false;
        	val = response.as<std::string>();
			
        	return true;
        }
		catch(...)
		{
            return false;
        }
    }
}