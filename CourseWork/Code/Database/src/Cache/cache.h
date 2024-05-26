#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <iostream>
#include <memory>
#include <mutex>

namespace database
{
    class Cache
    {
        private:
            std::shared_ptr<std::iostream> _stream;
			std::mutex _mtx;
            Cache();
        public:
			static Cache Instance;
            void insert(const std::string& key, const std::string& val, const std::string& lifetime);
            std::string get(const std::string& key);
    };
}

#endif