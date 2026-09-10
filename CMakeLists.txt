cmake_minimum_required(VERSION 3.15...${CMAKE_VERSION})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(elogpp VERSION "1.0.1.0" DESCRIPTION "CPP wrapper for elog" HOMEPAGE_URL "https://rpclab.github.io/elogpp" LANGUAGES CXX)

option(CXX_ELOG_OPENSSL "Use OpenSSL provided by the system" ON)
option(CXX_ELOG_APPS "Compile apps" ON)
option(CXX_ELOG_TESTS "Build the test executable" ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

include(GNUInstallDirs)

set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(source)

if(CXX_ELOG_APPS)
  add_subdirectory(apps)
endif()

if(CXX_ELOG_TESTS)
  include(CTest)
  add_subdirectory(tests)
endif()
