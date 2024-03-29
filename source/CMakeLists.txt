include(Jsoncpp)
include(OpenSSL)
include(Dotenv)

set(ELOGCPP_PUBLIC_HEADERS
  ${CMAKE_SOURCE_DIR}/include/ElogConfig.hpp
  ${CMAKE_SOURCE_DIR}/include/ElogEntry.hpp
  ${CMAKE_SOURCE_DIR}/include/ElogManager.hpp
  ${CMAKE_SOURCE_DIR}/include/ElogServer.hpp
  ${CMAKE_SOURCE_DIR}/include/ElogUser.hpp
  ${CMAKE_SOURCE_DIR}/include/Logbook.hpp
  ${CMAKE_SOURCE_DIR}/include/Connector.hpp
  ${CMAKE_SOURCE_DIR}/include/Crypt.hpp
  ${CMAKE_SOURCE_DIR}/include/Elog.hpp)

set(ELOGCPP_SOURCES
    ElogConfig.cpp
    ElogEntry.cpp
    ElogManager.cpp
    ElogServer.cpp
    ElogUser.cpp
    LogBook.cpp
    Crypt.cpp
    Elog.cpp
    Connector.cpp)

add_library(elogpp ${ELOGCPP_SOURCES})
target_compile_definitions(elogpp PRIVATE HAVE_SSL)
if(WIN32)
  target_link_libraries(elogpp PRIVATE wsock32 PRIVATE ws2_32 PRIVATE shlwapi)
  set_target_properties(elogpp PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"LIBCMTD\";\"LIBCMT\"")
endif()
target_include_directories(elogpp  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/elogpp>)

target_link_libraries(elogpp PUBLIC jsoncpp_static PRIVATE OpenSSL::SSL PRIVATE laserpants::dotenv)
set_target_properties(elogpp PROPERTIES PUBLIC_HEADER "${ELOGCPP_PUBLIC_HEADERS}")
add_library(elogpp::elogpp ALIAS elogpp)
install(TARGETS elogpp RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/elogpp")

