cmake_minimum_required(VERSION 3.6)
project(Serviceize)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories( dist/include )

set(SOURCE_FILES
        dist/include/Serviceize/ServiceController.h
		dist/include/Serviceize/Application.h
		dist/include/Serviceize/Process.h
		dist/include/Serviceize/AutoCloser.h
        src/ServiceController.cpp
        src/Application.cpp
		src/Process.cpp
		
        )

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# Output final binaries in the dist folder so they easily can be distributed together with the header files.
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/dist/bin/${CMAKE_BUILD_TYPE}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/dist/bin/${CMAKE_BUILD_TYPE}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/dist/bin/${CMAKE_BUILD_TYPE}"
        )