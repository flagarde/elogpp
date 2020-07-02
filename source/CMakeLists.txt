set(ELOGCPP_PUBLIC_HEADERS 
  include/ElogConfig.hpp  
  include/ElogEntry.hpp  
  include/ElogManager.hpp   
  include/ElogServer.hpp  
  include/ElogUser.hpp 
  include/Logbook.hpp
  include/Connector.hpp
  include/Crypt.hpp
  include/Elog.hpp
)

set(ELOGCPP_SOURCES  
  src/ElogConfig.cpp  
  src/ElogEntry.cpp  
  src/ElogManager.cpp  
  src/ElogServer.cpp  
  src/ElogUser.cpp  
  src/LogBook.cpp
  src/Crypt.cpp
  src/Elog.cpp
  src/Connector.cpp
)

include(jsoncpp)
include(openssl)
find_package(OpenSSL REQUIRED)

add_library(elogpp STATIC ${ELOGCPP_SOURCES})
target_compile_definitions(elogpp PRIVATE HAVE_SSL)
if (WIN32)
  target_link_libraries(elogpp PRIVATE wsock32 PRIVATE ws2_32 PRIVATE shlwapi)
  set_target_properties(elogpp PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"LIBCMTD\";\"LIBCMT\"")
endif()
target_include_directories(elogpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(elogpp PUBLIC jsoncpp::jsoncpp PUBLIC OpenSSL::SSL)
install(FILES ${ELOGCPP_PUBLIC_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/elogpp")

install(TARGETS elogpp ARCHIVE DESTINATION ${ARCHIVE_OUTPUT_DIR} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIR} LIBRARY DESTINATION ${LIBRARY_OUTPUT_DIR})
