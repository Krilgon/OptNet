cmake_minimum_required(VERSION 3.8)

project(GraphGenerator)

add_executable(GraphGenerator
    main.cpp
    GraphGenerator.cpp
    GraphGenerator.h
    JsonExporter.cpp
    JsonExporter.h
    Graph.h
    Node.h
    Edge.h
    NetworkData.h
)

if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
    set_property(TARGET GraphGenerator PROPERTY CXX_STANDARD 20)
endif()
