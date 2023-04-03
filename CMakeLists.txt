cmake_minimum_required(VERSION 3.24)
project(vox_converter)

set(CMAKE_CXX_STANDARD 20)

set(OpenVDB_INCLUDE_DIR "/usr/local/include")

list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/OpenVDB")
find_package(OpenVDB REQUIRED)


add_executable(vox_converter main.cpp)

target_link_libraries(vox_converter OpenVDB::openvdb)
