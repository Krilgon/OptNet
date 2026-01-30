cmake_minimum_required(VERSION 3.15)
project(GA LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GA
    main.cpp
    GA.cpp
    GraphLoader.cpp
    JsonExporter.cpp
    PathUtils.cpp
    Fitness.cpp
)

target_include_directories(GA PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
