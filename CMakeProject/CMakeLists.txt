project(DocumentManager)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(../DocumentManager SRC_DIR)
add_executable(DocumentManager ${SRC_DIR})