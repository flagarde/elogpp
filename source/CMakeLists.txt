if(CXX_ELOG_OPENSSL)
  find_package(OpenSSL COMPONENTS SSL REQUIRED)
endif()

add_library(elogpp
  Entry.cpp
  Manager.cpp
  Server.cpp
  Crypt.cpp
  Elog.cpp
  Connector.cpp
)

set(ELOGCPP_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/include/Entry.hpp
  ${PROJECT_SOURCE_DIR}/include/Manager.hpp
  ${PROJECT_SOURCE_DIR}/include/Server.hpp
  ${PROJECT_SOURCE_DIR}/include/User.hpp
  ${PROJECT_SOURCE_DIR}/include/Logbook.hpp
  ${PROJECT_SOURCE_DIR}/include/Connector.hpp
  ${PROJECT_SOURCE_DIR}/include/Crypt.hpp
  ${PROJECT_SOURCE_DIR}/include/Elog.hpp)

target_link_libraries(elogpp PRIVATE $<$<BOOL:${CXX_ELOG_OPENSSL}>:OpenSSL::SSL>)
target_compile_definitions(elogpp PRIVATE $<$<BOOL:${CXX_ELOG_OPENSSL}>:HAVE_SSL> OPENSSL_NO_DEPRECATED)
target_compile_features(elogpp PUBLIC cxx_std_17)
set_target_properties(elogpp PROPERTIES CXX_EXTENSIONS OFF PUBLIC_HEADER "${ELOGCPP_PUBLIC_HEADERS}")

if(WIN32)
  target_link_libraries(elogpp PRIVATE wsock32 PRIVATE ws2_32 PRIVATE shlwapi)
  set_target_properties(elogpp PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"LIBCMTD\";\"LIBCMT\"")
endif()
target_include_directories(elogpp PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/elogpp>)
add_library(elogpp::elogpp ALIAS elogpp)
install(TARGETS elogpp RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/elogpp")

