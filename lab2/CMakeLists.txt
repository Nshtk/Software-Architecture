cmake_minimum_required(VERSION 3.0.0)
project(lab2 VERSION 0.1.0)

set(SRC_LIST 	Services/UserService/Core/main.cpp
				Services/UserService/Database/database.cpp
				Services/UserService/Database/user.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 -Wall")

include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\ActiveRecord\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\MongoDB\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Prometheus\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Redis\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Zip\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Crypto\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Data\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\JWT\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Net\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\NetSSL_OpenSSL\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\CppUnit\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Encodings\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Foundation\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\JSON\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\Util\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\XML\\include)
include_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco-1.13.2-all\\openssl\\build\\include)
link_directories(C:\\Users\\Root\\Documents\\GitHub\\Labs\\SA\\poco\\lib)
add_executable(lab2 ${SRC_LIST} )

#find_package(Poco REQUIRED Foundation)
#target_link_libraries(lab2 PRIVATE Poco::Foundation)
