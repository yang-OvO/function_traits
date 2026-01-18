cmake_minimum_required(VERSION 3.10)

project(function_traits)

file(GLOB SOURCES "*.cpp")

foreach(SOURCE ${SOURCES})
    get_filename_component(PROGRAM ${SOURCE} NAME_WE)
    add_executable(${PROGRAM} ${SOURCE})
    target_compile_features(${PROGRAM} PRIVATE cxx_std_17)
endforeach()

