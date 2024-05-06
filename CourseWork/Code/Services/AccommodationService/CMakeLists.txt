cmake_minimum_required(VERSION 3.2)

project(accommodation_service)
SET (ACCOMMODATION_BINARY "accommodation_service")

SET(STD_CXX "c++17")
SET(CMAKE_CXX_STANDARD 17)
SET(REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX} ${REDISCPP_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -std=${STD_CXX} -Wall -DNDEBUG ${REDISCPP_FLAGS}")
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL)
find_package(Threads)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data JWT MongoDB)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

add_executable(${ACCOMMODATION_BINARY} ./src/main.cpp
							./src/Server/http_web_server.cpp
							./src/Server/Requests/http_request_factory.cpp
							./src/Server/Requests/Handlers/accommodation_handler.cpp
							../../Database/src/Config/config.cpp
							../../Database/src/Cache/cache.cpp
							../../Database/src/database.cpp
							../../Database/src/Models/Accommodation/accommodation.cpp
							../../Common/src/Utility/utility.cpp)


target_include_directories(${ACCOMMODATION_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${ACCOMMODATION_BINARY} PRIVATE -Wall -Wextra -pedantic)
set_target_properties(${ACCOMMODATION_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${ACCOMMODATION_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${ACCOMMODATION_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoDataPostgreSQL")
