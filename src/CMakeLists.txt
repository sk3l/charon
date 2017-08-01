project(charon)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -pedantic -D_GLIBCXX_CONCEPT_CHECKS -std=gnu++11")

include_directories("${CMAKE_HOME_DIRECTORY}")
include_directories("${PROJECT_SOURCE_DIR}/../lib/includes")

#find_library(LIBSSH, libssh)

find_library(LIBSSH
   NAMES
      ssh
      libssh
   PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      ${CMAKE_LIBRARY_PATH}
      ${CMAKE_INSTALL_PREFIX}/lib
)

if(NOT LIBSSH)
   message(FATAL_ERROR "libssh library not found")
endif()

#add_subdirectory(./lib)

#message("Home => ${CMAKE_HOME_DIRECTORY}")

set(
   INCLUDES
   ${PROJECT_SOURCE_DIR}/arg_parser.h
   ${PROJECT_SOURCE_DIR}/sftp_server.h
   ${PROJECT_SOURCE_DIR}/sftp_connection.h
   ${PROJECT_SOURCE_DIR}/sftp_directory.h
   ${PROJECT_SOURCE_DIR}/sftp_file.h
)

set(
   SRCFILES
   ${PROJECT_SOURCE_DIR}/arg_parser.cpp
   ${PROJECT_SOURCE_DIR}/main.cpp
   ${PROJECT_SOURCE_DIR}/sftp_server.cpp
   ${PROJECT_SOURCE_DIR}/sftp_connection.cpp
   ${PROJECT_SOURCE_DIR}/sftp_directory.cpp
   ${PROJECT_SOURCE_DIR}/sftp_file.cpp
)

#message("${CMAKE_HOME_DIRECTORY}/../build/")

add_executable(${PROJECT_NAME} ${INCLUDES} ${SRCFILES})
target_link_libraries(${PROJECT_NAME} ${LIBSSH})
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/../lib/libsk3l3tal.so)

