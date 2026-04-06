cmake_minimum_required(VERSION 3.10)
project(HelloWorldTest)

enable_testing()

add_executable(hello_world_test test_hello_world.cpp)

add_test(NAME HelloWorldTest COMMAND hello_world_test)