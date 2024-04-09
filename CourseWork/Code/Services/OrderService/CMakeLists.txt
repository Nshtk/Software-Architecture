cmake_minimum_required(VERSION 3.2)

project(ORDER_service)
SET (ORDER_BINARY "order_service")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL)
find_package(Threads)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data JWT MongoDB)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

add_executable(${ORDER_BINARY} ./src/main.cpp
							../../Database/src/Config/config.cpp
							../../Database/src/database.cpp
							../../Database/src/Models/Order/order.cpp)


target_include_directories(${ORDER_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${ORDER_BINARY} PRIVATE -Wall -Wextra -pedantic)
set_target_properties(${ORDER_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${ORDER_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${ORDER_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoDataPostgreSQL")
