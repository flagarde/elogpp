########################################################
#                                                      #
#            CMake file for building SJTUrpc           #
#            @author Lagarde François, SJTU            #
#                                                      #
########################################################
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)


# Name Project
PROJECT(SJTUrpc)

# Project Version 
SET( SJTURPC_VERSION_MAJOR 0 )
SET( SJTURPC_VERSION_MINOR 0 )
SET( SJTURPC_VERSION_PATCH 0 )

#Default Installation Folders
SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
SET(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")
SET(SUBMODULES_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/submodules")

#Submodules
#jsoncpp
OPTION(JSONCPP_WITH_TESTS "Compile and (for jsoncpp_check) run JsonCpp test executables" OFF)
OPTION(JSONCPP_WITH_POST_BUILD_UNITTEST "Automatically run unit-tests as a post build step" OFF)
OPTION(JSONCPP_WITH_PKGCONFIG_SUPPORT "Generate and install .pc files" OFF)
OPTION(JSONCPP_WITH_CMAKE_PACKAGE "Generate and install cmake package files" OFF)
ADD_SUBDIRECTORY(submodules/jsoncpp)

option(FMT_DOC "Generate the doc target." OFF)
option(FMT_INSTALL "Generate the install target." ON)
option(FMT_TEST "Generate the test target." OFF)
OPTION(BUILD_SHARED_LIBS "" OFF)
ADD_SUBDIRECTORY(submodules/fmt)


ADD_SUBDIRECTORY(elogcpp)
