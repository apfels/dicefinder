cmake_minimum_required( VERSION 3.20 )
project( dicefinder )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -flto")

add_executable( dicefinder
  src/main.cpp
  src/args.cpp
  src/topology.cpp
  src/weighting.cpp )

set_property(TARGET dicefinder PROPERTY CXX_STANDARD 23)

target_include_directories( dicefinder PRIVATE inc )
