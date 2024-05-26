#include "cache.h"
#include "../Config/config.h"

#include <exception>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

namespace database
{
	Cache Cache::Instance;
	
    Cache::Cache()
    {
        std::string host = Config::get().getCacheServers().substr(0, Config::get().getCacheServers().find(':'));
        std::string port = Config::get().getCacheServers().substr(Config::get().getCacheServers().find(':') + 1);

        std::cout << "Cache host:" << host <<" port:" << port << std::endl;
        _stream = rediscpp::make_stream(host, port);
    }

    std::string Cache::get(const std::string& key)
    {
        try
		{
        	std::lock_guard<std::mutex> lck(_mtx);
        	rediscpp::value response = rediscpp::execute(*_stream, "get", key);
	
        	if (response.empty() || response.is_error_message())
        	    return std::string();
				
        	return response.as<std::string>();;
        }
		catch(...)
		{
            return std::string();
        }
    }
    void Cache::insert(const std::string& key, const std::string& val, const std::string& lifetime)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        rediscpp::value response = rediscpp::execute(*_stream, "set", key, val, "ex", lifetime);
    }
}