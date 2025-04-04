cmake_minimum_required(VERSION 3.30)
project(n64-plus)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(BUILD_FOR_IOS "Build for iOS target" OFF)

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

if(BUILD_FOR_IOS)
    message(STATUS "Building for iOS")
    # Grab all source files
    file(GLOB_RECURSE EMU_SOURCES src/*.cpp)

    foreach(file ${EMU_SOURCES})
        if("${file}" MATCHES ".*/parallel-rdp-files/.*")
            list(REMOVE_ITEM EMU_SOURCES ${file})
        endif()
    endforeach()

    # Remove main.cpp from the list
    list(REMOVE_ITEM EMU_SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)
    list(REMOVE_ITEM EMU_SOURCES ${CMAKE_SOURC_DIR}/src/parallel-rdp-files/)
    add_library(N64Core STATIC ${EMU_SOURCES})

    set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.1.sdk)
    set(CMAKE_OSX_ARCHITECTURES arm64)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # required for iOS static libs

    # Optional: If targeting simulator
    # set(CMAKE_OSX_SYSROOT iphonesimulator)
    # set(CMAKE_OSX_ARCHITECTURES arm64 x86_64)

    # Link the framework binary directly
    target_link_libraries(N64Core
        "-framework Foundation"
        "-framework CoreFoundation"
    )

    set_target_properties(N64Core PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
else()
    message(STATUS "Building for MacOS")

    find_package(PkgConfig REQUIRED)
    find_package(Vulkan REQUIRED)
    pkg_check_modules(SDL3 REQUIRED sdl3)
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
endif()