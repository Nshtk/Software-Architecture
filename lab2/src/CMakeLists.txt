cmake_minimum_required(VERSION 3.2)

project(lab2)
SET (LAB2_BINARY "lab2")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL)
find_package(Threads)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data JWT MongoDB)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

add_executable(${LAB2_BINARY} 		../src/Services/UserService/Core/main.cpp
									../src/Services/UserService/Config/config.cpp
									../src/Services/UserService/Database/database.cpp
									../src/Services/UserService/Database/user.cpp)


target_include_directories(${LAB2_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${LAB2_BINARY} PRIVATE -Wall -Wextra -pedantic)
set_target_properties(${LAB2_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${LAB2_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${LAB2_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoDataPostgreSQL")
