cmake_minimum_required (VERSION 3.4)

set (CMAKE_C_COMPILER gcc)
set (CMAKE_CXX_COMPILER g++)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra -Weffc++ -O3")

project (app)
enable_language(CXX)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_VERBOSE_MAKEFILE TRUE)

set (SOURCE_FILES
    #receiver.cpp
    #transmitter.cpp
    main.cpp
    )

#add_executable (receiver ${SOURCE_FILES})
#add_executable (transmitter ${SOURCE_FILES})
add_executable (app ${SOURCE_FILES})
