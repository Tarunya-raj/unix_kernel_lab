#cmake_minimum_required(VERSION 3.16)

project(unix_kernel_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(unix_kernel_lab main.cpp
    include/buffer/delete.txt include/fileSubsystem/delete.txt include/kernel/ds/buffer.h include/kernel/ds/bufferCache.h include/kernel/delte.txt include/processSubsystem/delete.txt include/systemCalls/delete.txt src/buffer/delete.txt src/fileSubsystem/delete.txt src/kernel/ds/bufferCache.cpp src/kernel/delete.txt src/processSubsystem/delte.txt src/systemCalls/delete.txt CMakeLists.txt dummy_disk.img)

include(GNUInstallDirs)
install(TARGETS unix_kernel_lab
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

