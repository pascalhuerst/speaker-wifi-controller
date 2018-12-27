cmake_minimum_required(VERSION 2.8)

project(speaker-wifi-controller)
set(SOURCES main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-unused-parameter -Wall -O2")

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${SOURCES}
    )

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/sbin COMPONENT runtime)
