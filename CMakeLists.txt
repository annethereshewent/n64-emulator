cmake_minimum_required(VERSION 3.30)
project(n64-plus)

set (CMAKE_CXX_STANDARD 23)

find_package(PkgConfig REQUIRED)
find_package(Vulkan REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

include_directories()

add_executable(n64-plus ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(n64-plus ${SDL3_LIBRARIES} Vulkan::Vulkan)
target_include_directories(
    n64-plus PUBLIC ${SDL3_INCLUDE_DIRS}
    PUBLIC ${VULKAN_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parallel-rdp-standalone/util/
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parallel-rdp-standalone/volk/
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parallel-rdp-standalone/vulkan/
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parallel-rdp-standalone/parallel-rdp/
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parallel-rdp-standalone/vulkan-headers/include
)
target_compile_options(n64-plus PUBLIC ${SDL3_CFLAGS_OTHER})