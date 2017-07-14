project(charon)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -pedantic -D_GLIBCXX_CONCEPT_CHECKS -std=gnu++11")

include_directories("${CMAKE_HOME_DIRECTORY}")
include_directories("${PROJECT_SOURCE_DIR}/../lib/includes")

#add_subdirectory(./lib)

#message("Home => ${CMAKE_HOME_DIRECTORY}")
#set(
#INCLUDES
#${INCLUDES}
#PARENT_SCOPE
#)

set(
   SRCFILES
   ${PROJECT_SOURCE_DIR}/main.cpp 
)

#message("${CMAKE_HOME_DIRECTORY}/../build/")

add_executable(${PROJECT_NAME} ${SRCFILES}) 
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/../lib/libsk3l3tal.so)

