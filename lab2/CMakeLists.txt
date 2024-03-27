cmake_minimum_required(VERSION 3.0.0)
project(lab2 VERSION 0.1.0)

set(SRC_LIST 	Services/UserService/Core/main.cpp
				Services/UserService/Database/database.cpp
				Services/UserService/Database/user.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 -Wall")

include_directories(C:\\1Dev\\vcpkg\\installed\\x64-windows\\include)
link_directories(C:\\1Dev\\vcpkg\\installed\\x64-windows\\lib)
add_executable(lab2 ${SRC_LIST} )

find_package(Poco REQUIRED Foundation)
target_link_libraries(lab2 PRIVATE Poco::Foundation)
