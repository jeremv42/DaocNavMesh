cmake_minimum_required(VERSION 3.5)
project(DaocNavMesh)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

# start with deps
add_subdirectory(glm)

add_subdirectory(niflib)
set(niflib_headers niflib/include)
add_subdirectory(recast)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS src/*.hpp src/*.h)

add_executable(DaocNavMesh ${SOURCES} ${HEADERS})
set_property(TARGET DaocNavMesh PROPERTY CXX_STANDARD 23)
target_link_libraries(DaocNavMesh niflib_static Recast glm)
target_include_directories(DaocNavMesh PRIVATE ${niflib_headers} ${Recast_INCLUDE_DIR})
