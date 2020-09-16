cmake_minimum_required(VERSION 3.7)
project(minirogue)

file(GLOB SOURCE_FILES    
    "*.cpp"
) 
#set(SOURCE_FILES main.cpp sdl_render.cpp vmath.cpp bitfont.cpp animation.cpp)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Find SDL2 and OpenGL
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED COMPONENTS main)
find_package(SDL2_image REQUIRED)


include_directories(${SDL2_INCLUDE_DIRS} ${SDL2main_INCLUDE_DIRS}  ${SDL2_IMAGE_INCLUDE_DIRS} ${CMAKE_BINARY_DIR})

add_executable(minirogue ${SOURCE_FILES})
target_link_libraries(minirogue ${SDL2_LIBS} ${SDL2_IMAGE_LIBRARIES})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../")   