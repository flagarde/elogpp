########################################################
#                                                      #
#            CMake file for building elogpp            #
#            @author Lagarde François, SJTU            #
#                                                      #
########################################################
cmake_minimum_required(VERSION 3.9...3.16.1 FATAL_ERROR)

project(elogpp VERSION "1.1.0.0" DESCRIPTION "CPP wrapper for elog" HOMEPAGE_URL "https://rpclab.github.io/elogpp" LANGUAGES CXX)

option(COMPILE_ELOG "Compile elog" ON)
option(BUILD_TEST "Build the test executable" ON)

# ==== Define language standard configurations requiring at least c++11 standard
if(CMAKE_CXX_STANDARD EQUAL "98")
    message(FATAL_ERROR "CMAKE_CXX_STANDARD:STRING=98 is not supported.")
endif()

set(ELOG_PATH $CACHE{ELOG_PATH})
if(NOT ${ELOG_PATH} STREQUAL "")
  message(STATUS "Elog path : ${ELOG_PATH}")
endif()

#####
##  Set the default target properties
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17) # Supported values are ``11``, ``14``, and ``17``.
endif()
if(NOT CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/bin" CACHE PATH "Install Prefix" FORCE)
endif()

set(GENERATED_HEADER_PATH ${CMAKE_BINARY_DIR}/generated/)

include(ExternalProject)

if (NOT TARGET jsoncpp)
  set( jsoncpp_repository "https://github.com/RPClab/jsoncpp" )
  set( jsoncpp_version "master" )
  option(JSONCPP_WITH_TESTS "Compile and (for jsoncpp_check) run JsonCpp test executables" OFF)
  option(JSONCPP_WITH_POST_BUILD_UNITTEST "Automatically run unit-tests as a post build step" OFF)
  option(JSONCPP_WITH_WARNING_AS_ERROR "Force compilation to fail if a warning occurs" OFF)
  option(JSONCPP_WITH_STRICT_ISO "Issue all the warnings demanded by strict ISO C and ISO C++" ON)
  option(JSONCPP_WITH_PKGCONFIG_SUPPORT "Generate and install .pc files" OFF)
  option(JSONCPP_WITH_CMAKE_PACKAGE "Generate and install cmake package files" ON)
  option(JSONCPP_WITH_EXAMPLE "Compile JsonCpp example" OFF)
  option(BUILD_SHARED_LIBS "Build jsoncpp_lib as a shared library." OFF)
  # ----- jsoncpp package -----
  ExternalProject_Add( 
                      jsoncpp
                      GIT_REPOSITORY ${jsoncpp_repository}
                      GIT_TAG ${jsoncpp_version}
                      GIT_PROGRESS TRUE
                      GIT_SHALLOW TRUE
                      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DJSONCPP_WITH_TESTS=${JSONCPP_WITH_TESTS} -DJSONCPP_WITH_POST_BUILD_UNITTEST=${JSONCPP_WITH_POST_BUILD_UNITTEST} -DJSONCPP_WITH_PKGCONFIG_SUPPORT=${JSONCPP_WITH_PKGCONFIG_SUPPORT} -DJSONCPP_WITH_CMAKE_PACKAGE=${JSONCPP_WITH_CMAKE_PACKAGE} -DJSONCPP_WITH_EXAMPLE=${JSONCPP_WITH_EXAMPLE} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
                      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp-prefix
                      SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp
                      INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                      )
endif()

if (NOT TARGET pstreams)
  set(pstreams_repository "https://github.com/RPClab/pstreams" )
  set( pstreams_version "master" )
  # ----- pstreams package -----
  ExternalProject_Add( 
                      pstreams
                      GIT_REPOSITORY ${pstreams_repository}
                      GIT_TAG ${pstreams_version}
                      GIT_PROGRESS TRUE
                      GIT_SHALLOW TRUE
                      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
                      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/pstreams-prefix
                      SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/pstreams
                      INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                      )
endif()

configure_file("${PROJECT_SOURCE_DIR}/confs/Elog.json" "${CMAKE_INSTALL_PREFIX}/confs/Elog_DEMO.json" COPYONLY)

add_subdirectory(source)
