cmake_minimum_required(VERSION 3.2)

project(user_service)
SET (MY_BINARY "user_service")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL)
find_package(Threads)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data JWT MongoDB)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

add_executable(${MY_BINARY} ./src/main.cpp
							../../Database/src/Config/config.cpp
							../../Database/src/database.cpp
							../../Database/src/Models/User/user.cpp)


target_include_directories(${MY_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${MY_BINARY} PRIVATE -Wall -Wextra -pedantic)
set_target_properties(${MY_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${MY_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${MY_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoDataPostgreSQL")
