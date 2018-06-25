########################################################
#                                                      #
#            CMake file for building elogcpp           #
#            @author Lagarde François, SJTU            #
#                                                      #
########################################################
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)


# Name Project
PROJECT(elogcpp)
# Project Version 
SET( ELOGCPP_VERSION_MAJOR 1 )
SET( ELOGCPP_VERSION_MINOR 0 )
SET( ELOGCPP_VERSION_PATCH 0 )

# c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#option
option(BUILD_TEST "Build the test executable" ON)

# find files
file(GLOB ELOGCPP_PUBLIC_HEADERS include/*.h)
file(GLOB ELOGCPP_SOURCES src/*.cpp)
set(PSTREAMS_HEADER "${SUBMODULES_PREFIX}/pstreams")

install(FILES ${ELOGCPP_PUBLIC_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/elogpp")
ADD_LIBRARY( elogcpp STATIC ${ELOGCPP_SOURCES} )
target_include_directories(elogcpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_INSTALL_PREFIX}/include/" "${PSTREAMS_HEADER}/")
TARGET_LINK_LIBRARIES( elogcpp jsoncpp)
INSTALL( TARGETS elogcpp ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(BUILD_TEST)
    ADD_EXECUTABLE( test_elogcpp test_elogcpp.cpp ${ELOGCPP_PUBLIC_HEADERS} ${ELOGCPP_SOURCES} "${PSTREAMS_HEADER}/pstream.h")
    target_include_directories(test_elogcpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_INSTALL_PREFIX}/include/" "${PSTREAMS_HEADER}/")
    TARGET_LINK_LIBRARIES(test_elogcpp jsoncpp )
    install(TARGETS test_elogcpp  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
