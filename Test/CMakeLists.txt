cmake_minimum_required(VERSION 3.6)
project(Test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories( "${CMAKE_CURRENT_LIST_DIR}/../externals" )
include_directories( "${CMAKE_CURRENT_LIST_DIR}/../Serviceize/dist/include" )

set(SOURCE_FILES
        main.cpp
		test.cpp
		TestApp.cpp
		TestApp.h
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries( ${PROJECT_NAME} Serviceize )

# Output final binaries in the dist folder so they easily can be distributed together with the header files.
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/dist/bin/${CMAKE_BUILD_TYPE}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/dist/bin/${CMAKE_BUILD_TYPE}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/dist/bin/${CMAKE_BUILD_TYPE}"
        )