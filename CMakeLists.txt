########################################################
#                                                      #
#            CMake file for building elogpp            #
#            @author Lagarde François, SJTU            #
#                                                      #
########################################################
cmake_minimum_required(VERSION 3.15...3.17.3)

project(elogpp VERSION "1.1.0.0" DESCRIPTION "CPP wrapper for elog" HOMEPAGE_URL "https://rpclab.github.io/elogpp" LANGUAGES CXX)

option(COMPILE_ELOG "Compile elog" ON)
option(BUILD_TEST "Build the test executable" ON)
option(USE_SYSTEM_OPENSSL "Use the OPENSSL provided by the system" ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(CMAKE_LINK_DEPENDS_NO_SHARED TRUE)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/bin" CACHE PATH "Install Prefix" FORCE)
endif()

include(GNUInstallDirs)
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  set(LIBRARY_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
  set(RUNTIME_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
  set(ARCHIVE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
  set(INCLUDE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
  set(CMAKE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake")
else()
  set(LIBRARY_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/lib")
  set(RUNTIME_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}")
  set(ARCHIVE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/lib")
  set(INCLUDE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/include")
  set(CMAKE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/cmake")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts" "${CMAKE_CURRENT_SOURCE_DIR}/external" "${CMAKE_CURRENT_SOURCE_DIR}")
link_directories("${CMAKE_INSTALL_PREFIX}/lib")
# jsoncpp
set(JSONCPP_REPOSITORY "https://github.com/open-source-parsers/jsoncpp.git")
set(JSONCPP_VERSION "master")

set(OPENSSL_REPOSITORY "https://github.com/flagarde/openssl-cmake.git")
set(OPENSSL_TAG "master")

configure_file("${PROJECT_SOURCE_DIR}/confs/Elog.json" "${CMAKE_INSTALL_PREFIX}/confs/Elog_DEMO.json" COPYONLY)

add_subdirectory(source)

if(COMPILE_ELOG)
  add_subdirectory(apps)
endif()

if(BUILD_TEST)
  add_subdirectory(tests)
endif()
