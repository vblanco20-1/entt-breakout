cmake_minimum_required(VERSION 3.2)
project(breakout)



set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


add_subdirectory(third_party EXCLUDE_FROM_ALL)
add_subdirectory(src)



