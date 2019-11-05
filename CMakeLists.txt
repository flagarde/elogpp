########################################################
#                                                      #
#            CMake file for building elogpp            #
#            @author Lagarde François, SJTU            #
#                                                      #
########################################################
cmake_minimum_required(VERSION 3.9..3.15 FATAL_ERROR)


# Name Project
PROJECT(elogpp VERSION 1.1.0.0 DESCRIPTION "CPP wrapper for elog" HOMEPAGE_URL "https://rpclab.github.io/elogpp" LANGUAGES CXX)

# c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INSTALL_PREFIX "")
if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin)
else()
endif()

include(ExternalProject)

set( jsoncpp_repository "https://github.com/RPClab/jsoncpp" )
set( jsoncpp_version "master" )
OPTION(JSONCPP_WITH_TESTS "Compile and (for jsoncpp_check) run JsonCpp test executables" OFF)
OPTION(JSONCPP_WITH_POST_BUILD_UNITTEST "Automatically run unit-tests as a post build step" OFF)
OPTION(JSONCPP_WITH_PKGCONFIG_SUPPORT "Generate and install .pc files" OFF)
OPTION(JSONCPP_WITH_CMAKE_PACKAGE "Generate and install cmake package files" ON)
# ----- jsoncpp package -----
ExternalProject_Add( 
                    jsoncpp
                    GIT_REPOSITORY ${jsoncpp_repository}
                    GIT_TAG ${jsoncpp_version}
                    GIT_PROGRESS TRUE
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DJSONCPP_WITH_TESTS=${JSONCPP_WITH_TESTS} -DJSONCPP_WITH_POST_BUILD_UNITTEST=${JSONCPP_WITH_POST_BUILD_UNITTEST} -DJSONCPP_WITH_PKGCONFIG_SUPPORT=${JSONCPP_WITH_PKGCONFIG_SUPPORT} -DJSONCPP_WITH_CMAKE_PACKAGE=${JSONCPP_WITH_CMAKE_PACKAGE}
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp-prefix
                    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp
                    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                    )

set( pstreams_repository "https://github.com/RPClab/pstreams" )
set( pstreams_version "master" )
# ----- pstreams package -----
ExternalProject_Add( 
                    pstreams
                    GIT_REPOSITORY ${pstreams_repository}
                    GIT_TAG ${pstreams_version}
                    GIT_PROGRESS TRUE
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
 		    #CONFIGURE_COMMAND ""
		    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/pstreams/pstream.h ${CMAKE_INSTALL_PREFIX}/include/pstreams/pstream.h
		    BUILD_COMMAND ""
  		    INSTALL_COMMAND ""
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/pstreams-prefix
                    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/pstreams
                    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                    )

ADD_SUBDIRECTORY(source)
