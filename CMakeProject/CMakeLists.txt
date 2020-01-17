project(DocumentManager)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(../DocumentManager SRC_DIR)
add_executable(DocumentManager ${SRC_DIR})

if(defined ${CMAKE_INSTALL_BINDIR})
message("use default install directory")
set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR})
else()
message("default install directory not defined")
set(INSTALL_BIN_DIR "bin")
endif()

install(TARGETS DocumentManager RUNTIME DESTINATION ${INSTALL_BIN_DIR})