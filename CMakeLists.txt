cmake_minimum_required(VERSION 3.10)

project(draw_fractals)

add_definitions("-std=c++11")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
 conan_basic_setup()

 add_executable(app 
	"main.cpp" 
	"include/base.hpp"
	"include/pythagoras_tree.hpp" 
	"include/l_system_render.hpp")

 target_link_libraries(app ${CONAN_LIBS} Threads::Threads)