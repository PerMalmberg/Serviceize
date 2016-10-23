cmake_minimum_required(VERSION 3.6)
project(Main)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#add_definitions( -DUNICODE -D_UNICODE )

add_subdirectory( Serviceize )
add_subdirectory( Test )

add_dependencies(Test Serviceize)