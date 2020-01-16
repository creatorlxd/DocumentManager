project(DocumentManager)
cmake_minimum_required(VERSION 3.11)

aux_source_directory(../DocumentManager SRC_DIR)
add_executable(DocumentManager ${SRC_DIR})