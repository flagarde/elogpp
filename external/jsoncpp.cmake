if(USE_SYSTEM_JSONCPP)
  find_package(jsoncpp QUIET)
  if(jsoncpp_FOUND)
    set(COMPILE_JSONCPP FALSE)
  else()
    set(COMPILE_JSONCPP TRUE)
    message(STATUS "jsoncpp not found ! jsoncpp will be compiled !") 
  endif()
else()
  set(COMPILE_JSONCPP TRUE)
  message(STATUS "jsoncpp will be compiled") 
endif()

if(COMPILE_JSONCPP)
if (NOT TARGET jsoncpp_project)
include(ExternalProject)
# ----- jsoncpp_project package -----
  ExternalProject_Add(jsoncpp_project
                      GIT_REPOSITORY ${JSONCPP_REPOSITORY}
                      GIT_TAG ${JSONCPP_VERSION}
                      GIT_PROGRESS TRUE
                      GIT_SHALLOW TRUE
                      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD} -DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED} -DCMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS} -DJSONCPP_WITH_TESTS=FALSE -DJSONCPP_WITH_POST_BUILD_UNITTEST=FALSE -DJSONCPP_WITH_PKGCONFIG_SUPPORT=FALSE -DJSONCPP_WITH_EXAMPLE=FALSE -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE} -DCMAKE_INSTALL_LIBDIR=${ARCHIVE_OUTPUT_DIR}
                      PREFIX ${CMAKE_BINARY_DIR}/jsoncpp_project
                      INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                      LOG_DOWNLOAD ON
                      UPDATE_DISCONNECTED ON
                    )
  add_library(jsoncpp_internal INTERFACE)
  add_dependencies(jsoncpp_internal jsoncpp_project)
  target_link_libraries(jsoncpp_internal INTERFACE jsoncpp)
  target_include_directories(jsoncpp_internal INTERFACE "${INCLUDE_OUTPUT_DIR}/json")
  add_library(jsoncpp::jsoncpp ALIAS jsoncpp_internal)
endif()
else()
  add_library(jsoncpp_internal INTERFACE)
  target_link_libraries(jsoncpp_internal INTERFACE  jsoncpp)
  #get_target_property(JSON_INC_PATH jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories(jsoncpp_internal INTERFACE "${INCLUDE_OUTPUT_DIR}/json")
  add_library(jsoncpp::jsoncpp ALIAS jsoncpp_internal)
endif()
