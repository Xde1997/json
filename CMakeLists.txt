cmake_minimum_required(VERSION 3.22)

project(${CMAKE_CURRENT_SOURCE_DIR} CXX)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
file(GLOB HEADERSOURCES "*.hpp")

add_executable(json ${SOURCES} ${HEADERS} ${HEADERSOURCES})