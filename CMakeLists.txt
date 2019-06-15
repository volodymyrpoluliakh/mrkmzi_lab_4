cmake_minimum_required(VERSION 2.8)

project(rsa)
add_executable(${PROJECT_NAME} main.cpp big_int.cpp big_int.h functions.cpp functions.h)

target_link_libraries(${PROJECT_NAME} boost_program_options.a)
