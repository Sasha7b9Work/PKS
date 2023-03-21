# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

cmake_minimum_required(VERSION 2.9)

# Set project name
project (RES-Network-GUI)

set (TARGET_NAME RES-Network-GUI)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

add_subdirectory(Client-PC)
add_subdirectory(Device)
