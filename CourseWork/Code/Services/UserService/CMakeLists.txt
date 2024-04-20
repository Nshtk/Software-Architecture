cmake_minimum_required(VERSION 3.2)

project(user_service)
SET (USER_BINARY "user_service")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL)
find_package(Threads)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data JWT MongoDB)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

add_executable(${USER_BINARY} ./src/main.cpp
							./src/Server/http_web_server.cpp
							./src/Server/Requests/http_request_factory.cpp
							./src/Server/Requests/Handlers/user_handler.cpp
							../../Database/src/Config/config.cpp
							../../Database/src/database.cpp
							../../Database/src/Models/User/user.cpp
							../../Common/src/Utility/utility.cpp)


target_include_directories(${USER_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${USER_BINARY} PRIVATE -Wall -Wextra -pedantic)
set_target_properties(${USER_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${USER_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${USER_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoDataPostgreSQL")
