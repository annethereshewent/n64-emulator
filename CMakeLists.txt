cmake_minimum_required(VERSION 3.30)
project(n64-plus)

set (CMAKE_CXX_STANDARD 23)

find_package(PkgConfig REQUIRED)
find_package(Vulkan REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_executable(n64-plus ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(n64-plus ${SDL2_LIBRARIES} Vulkan::Vulkan)
target_include_directories(n64-plus PUBLIC ${SDL2_INCLUDE_DIRS} PUBLIC ${VULKAN_INCLUDE_DIRS})
target_compile_options(n64-plus PUBLIC ${SDL2_CFLAGS_OTHER})