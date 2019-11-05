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

#Default Installation Folders
SET(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/bin")
SET(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}")
SET(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")

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
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_FLAGS="-fPIC" -DJSONCPP_WITH_TESTS=${JSONCPP_WITH_TESTS} -DJSONCPP_WITH_POST_BUILD_UNITTEST=${JSONCPP_WITH_POST_BUILD_UNITTEST} -DJSONCPP_WITH_PKGCONFIG_SUPPORT=${JSONCPP_WITH_PKGCONFIG_SUPPORT} -DJSONCPP_WITH_CMAKE_PACKAGE=${JSONCPP_WITH_CMAKE_PACKAGE}
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp-prefix
                    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp
                    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                    )




SET(SUBMODULES_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/submodules")

#Submodules
#jsoncpp

#ADD_SUBDIRECTORY(submodules/jsoncpp)

#option(FMT_DOC "Generate the doc target." OFF)
#option(FMT_INSTALL "Generate the install target." ON)
#option(FMT_TEST "Generate the test target." OFF)
#OPTION(BUILD_SHARED_LIBS "" OFF)
#ADD_SUBDIRECTORY(submodules/fmt)


ADD_SUBDIRECTORY(source)
