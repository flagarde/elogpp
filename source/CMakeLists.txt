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
)

include(jsoncpp)
include(openssl)

add_library(Connector STATIC src/Connector.cpp)
add_dependencies(Connector openssl_project)
target_include_directories(Connector PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_compile_definitions(Connector PRIVATE HAVE_SSL)
target_link_libraries(Connector PRIVATE OpenSSL::SSL PRIVATE OpenSSL::Crypto)

add_library(elogpp STATIC ${ELOGCPP_SOURCES})
add_dependencies(elogpp jsoncpp_project Connector)
target_include_directories(elogpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"  PUBLIC "${CMAKE_INSTALL_PREFIX}/include/json")
target_link_libraries(elogpp INTERFACE jsoncpp PUBLIC Connector)
install(FILES ${ELOGCPP_PUBLIC_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/elogpp")

install(TARGETS elogpp ARCHIVE DESTINATION ${ARCHIVE_OUTPUT_DIR} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIR} LIBRARY DESTINATION ${LIBRARY_OUTPUT_DIR})
