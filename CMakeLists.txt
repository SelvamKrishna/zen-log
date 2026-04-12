cmake_minimum_required(VERSION 3.16)

project(zen-log LANGUAGES CXX)

add_library(zen-log INTERFACE)
add_library(zen::log ALIAS zen-log)

target_include_directories(zen-log INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(zen-log INTERFACE cxx_std_20)
