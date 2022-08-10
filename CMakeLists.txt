cmake_minimum_required( VERSION 3.20 )
project( dicefinder )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable( dicefinder
  src/main.cpp
  src/args.cpp )

set_property(TARGET dicefinder PROPERTY CXX_STANDARD 23)

target_include_directories( dicefinder PRIVATE inc )
