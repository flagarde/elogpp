cmake_minimum_required(VERSION 2.6)
project(sjturpc)

add_executable(sjturpc main.cpp)

install(TARGETS sjturpc RUNTIME DESTINATION bin)
