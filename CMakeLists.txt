cmake_minimum_required(VERSION 3.20)

project(MemoryPool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/memory_pool.cpp
    src/test.cpp
)

add_executable(test_exe ${SOURCES})

# 添加 include/ 以便查找对外头文件
target_include_directories(test_exe PRIVATE src ${PROJECT_SOURCE_DIR}/include)

if(WIN32)
    set_target_properties(test_exe PROPERTIES WIN32_EXECUTABLE FALSE)
endif()

if(MSVC)
    target_compile_options(test_exe PRIVATE /W4)
else()
    target_compile_options(test_exe PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(test_exe PRIVATE -g)
else()
    target_compile_options(test_exe PRIVATE -O2)
endif()