cmake_minimum_required(VERSION 3.16)

project(TrabajoClase)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp ")

find_package(fmt CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

add_executable(trabajo_clase main.cpp escala_grises.cpp)

target_link_libraries(trabajo_clase PRIVATE 
    fmt::fmt
    SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System)