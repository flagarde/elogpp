cmake_minimum_required(VERSION 3.15...3.17.3)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GetCMakeMM)
cmmm(VERSION "2.0" REPOSITORY "flagarde/CMakeMM" PROVIDER "gitlab" VERBOSITY VERBOSE)

cmmm_modules_list(REPOSITORY "flagarde/CMakeCM" PROVIDER "gitlab")

project(elogpp VERSION "1.9.0.0" DESCRIPTION "CPP wrapper for elog" HOMEPAGE_URL "https://rpclab.github.io/elogpp" LANGUAGES CXX)

option(COMPILE_ELOG "Compile elog" ON)
option(BUILD_TESTS "Build the test executable" ON)

option(USE_SYSTEM_OPENSSL "Use OpenSSL provided by the system" OFF)
option(USE_SYSTEM_JSONCPP "Use jsoncpp provided by the system" OFF)

include(Jsoncpp)
include(OpenSSL)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(CMAKE_LINK_DEPENDS_NO_SHARED TRUE)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD OFF)
endif()

include(GNUInstallDirs)

set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories("${CMAKE_INSTALL_FULL_LIBDIR}")

add_subdirectory(source)

if(COMPILE_ELOG)
  add_subdirectory(apps)
endif()

if(BUILD_TESTS)
  include(CTest)
  add_subdirectory(tests)
endif()
