cmake_minimum_required(VERSION 3.2)

project(accommodation_service)
SET (ACCOMMODATION_BINARY "accommodation_service")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL)
find_package(Threads)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data JWT MongoDB)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

add_executable(${ACCOMMODATION_BINARY} ./src/main.cpp
							../../Database/src/Config/config.cpp
							../../Database/src/database.cpp
							../../Database/src/Models/Accommodation/accommodation.cpp)


target_include_directories(${ACCOMMODATION_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${ACCOMMODATION_BINARY} PRIVATE -Wall -Wextra -pedantic)
set_target_properties(${ACCOMMODATION_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${ACCOMMODATION_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${ACCOMMODATION_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoDataPostgreSQL")
