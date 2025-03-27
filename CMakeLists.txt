cmake_minimum_required(VERSION 3.30)
project(n64-plus)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

set (CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-O3")

if (ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "-fsanitize=address -fno-omit-frame-pointer -g")
    set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize-address-use-after-scope -g")
    set(CMAKE_C_FLAGS "-fsanitize=address -fno-omit-frame-pointer -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_UBSAN)
    message(STATUS "UndefinedBehaviorSanitizer enabled!")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
endif()

find_package(PkgConfig REQUIRED)
find_package(Vulkan REQUIRED)
find_package(OpenSSL REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

include_directories()

add_executable(n64-plus ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(n64-plus ${SDL3_LIBRARIES} Vulkan::Vulkan OpenSSL::SSL)
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