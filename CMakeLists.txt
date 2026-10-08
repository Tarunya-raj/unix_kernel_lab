cmake_minimum_required(VERSION 3.16)

project(unix_kernel_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Default to Release so -O2/-O3 is used unless told otherwise
if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_sources(${PROJECT_NAME} PRIVATE 	src/kernel/ds/bufferCache.cpp)

#g++ -I/path/to/MyProject/include/kernel/ds ...   -I include this directory to header search path.
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include/kernel/ds)


