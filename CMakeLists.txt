cmake_minimum_required(VERSION 3.21)
project(DatabaseC_)

set(CMAKE_CXX_STANDARD 14)

include_directories(Database)

add_executable(DatabaseC_
        Database/Database.cpp
        Database/Database.h)
