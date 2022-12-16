cmake_minimum_required(VERSION 3.22)

project(json CXX)

# 指定为C++17版本
set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
file(GLOB HEADERSOURCES "*.hpp")

add_executable(json ${SOURCES} ${HEADERS} ${HEADERSOURCES})