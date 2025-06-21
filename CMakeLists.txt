cmake_minimum_required(VERSION 3.27.7)


# defining the project
project(server++ VERSION 0.0.1)


# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS false)

# source files

set(SOURCES 
    src/main.cpp
    src/utils.cpp
    src/http.cpp
    src/logger.cpp
)


# adding the executable
add_executable(HttpServer ${SOURCES})

# include directory
target_include_directories(HttpServer PRIVATE include)