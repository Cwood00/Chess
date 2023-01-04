cmake_minimum_required(VERSION 3.23)
project(chess)

set(CMAKE_CXX_STANDARD 20)

add_executable(chess main.cpp GameState.cpp GameState.h)
