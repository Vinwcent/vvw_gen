cmake_minimum_required(VERSION 3.14)
project(CMakeProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Lib
include(sources.cmake)
set(VVW_GEN_LIBRARY_NAME vvw_gen)
add_library(${VVW_GEN_LIBRARY_NAME} ${VVW_GEN_FILES})
add_library(vvw_gen::vvw_gen ALIAS ${VVW_GEN_LIBRARY_NAME})
target_include_directories(${VVW_GEN_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(${VVW_GEN_LIBRARY_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(vvw_gen_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/src 
    ${vvw_gen_INCLUDE_DIRS} 
)
set(vvw_gen_INCLUDE_DIRS ${vvw_gen_INCLUDE_DIRS} PARENT_SCOPE)


option(BUILD_VVW_TESTS OFF "Build test suite")
if (BUILD_VVW_TESTS)
	add_subdirectory(tests)
endif()
