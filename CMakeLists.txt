cmake_minimum_required(VERSION 3.23)

project(unix_kernel_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(bufferCache)
target_sources(bufferCache PRIVATE src/kernel/ds/bufferCache.cpp
		include/kernel/ds/buffer.h
		PUBLIC
		FILE_SET bufferfiles
		TYPE HEADERS
		FILES
		include/kernel/ds/bufferCache.h
			
		)
target_include_directories(bufferCache PRIVATE ${PROJECT_SOURCE_DIR}/include)
add_library(fileSystem)
target_sources(fileSystem PRIVATE src/fileSubsystem/disk/fileRepresentation.cpp
			src/fileSubsystem/incore/incoreinode.cpp
			PUBLIC
			FILE_SET filesystemFiles
			TYPE HEADERS
			FILES
			include/fileSubsystem/disk/fileRepresentation.h	)
		target_include_directories(fileSystem PRIVATE ${PROJECT_SOURCE_DIR}/include)


		
add_executable(kernel)
target_sources(kernel PRIVATE app/main.cpp)
target_include_directories(kernel PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(kernel fileSystem bufferCache)
