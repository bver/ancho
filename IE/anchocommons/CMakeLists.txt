cmake_minimum_required (VERSION 2.8)
project (anchocommons)

set(${PROJECT_NAME}_INC_DIR ${${PROJECT_NAME}_SOURCE_DIR} CACHE PATH "Path to ${PROJECT_NAME} include directory")
set(${PROJECT_NAME}_SRC_DIR ${${PROJECT_NAME}_SOURCE_DIR})

include_directories(${${PROJECT_NAME}_INC_DIR})

set(${PROJECT_NAME}_HEADERS
  "${${PROJECT_NAME}_INC_DIR}/anchocommons.h"
  "${${PROJECT_NAME}_INC_DIR}/Exceptions.h"
  )

set(${PROJECT_NAME}_SOURCES 
  "${${PROJECT_NAME}_SRC_DIR}/strings.cpp"
  "${${PROJECT_NAME}_SRC_DIR}/utilities.cpp"
  )

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCES})

# set UNICODE
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS _UNICODE,UNICODE)

set(${PROJECT_NAME}_LIB_DIR "${${PROJECT_NAME}_BINARY_DIR}"  CACHE PATH "Path to ${PROJECT_NAME} lib directory")

message(STATUS "Configuring ${PROJECT_NAME}")
