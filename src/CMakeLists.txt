cmake_minimum_required(VERSION 3.7)
project(breakout)

set(SOURCE_FILES main.cpp)

# Find SDL2 and OpenGL
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED COMPONENTS main)


include_directories(${SDL2_INCLUDE_DIRS} ${SDL2main_INCLUDE_DIRS} ${CMAKE_BINARY_DIR})

add_executable(breakout ${SOURCE_FILES})
target_link_libraries(breakout ${SDL2_LIBS})

# Define executable target
#include_directories(${SDL2_INCLUDE_DIRS} ${SDL2main_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${CMAKE_BINARY_DIR})
#add_executable(CMakeDemo Main.cpp gl2/Context.cpp)
#target_link_libraries(CMakeDemo ${SDL2_LIBS} ${OPENGL_LIBRARIES})

# Copy SDL2 DLLs to output folder on Windows
if(WIN32)
    foreach(DLL ${SDL2_DLLS})
        add_custom_command(TARGET breakout POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:breakout>)
    endforeach()
endif()


#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIRS})



#install(TARGETS divider DESTINATION ${DIVISIBLE_INSTALL_BIN_DIR})
