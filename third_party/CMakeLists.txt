cmake_minimum_required(VERSION 3.2)


add_subdirectory(entt)

add_library(imgui STATIC)

# OpenGL
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED COMPONENTS main)


 set(imgui_sources 
   "imgui/imgui.cpp"
"imgui/imgui_sdl.cpp"
   "imgui/imgui_demo.cpp"
   "imgui/imgui_draw.cpp"   
   "imgui/imgui_widgets.cpp"
   "imgui/examples/imgui_impl_opengl2.cpp"
   "imgui/examples/imgui_impl_sdl.cpp"
   "imgui/misc/cpp/imgui_stdlib.cpp"
   )
   

target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2main_INCLUDE_DIRS})

   target_include_directories(imgui PUBLIC "imgui")
   
target_link_libraries(imgui PUBLIC opengl32  ${SDL2_LIBS})
target_sources(imgui PRIVATE ${imgui_sources})