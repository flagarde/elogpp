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

add_library(Connector STATIC src/Connector.cpp)
find_package(OpenSSL QUIET) 
if(OpenSSL_FOUND)
  target_include_directories(Connector PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
  target_compile_definitions(Connector PRIVATE HAVE_SSL)
  target_link_libraries(Connector PRIVATE  OpenSSL::Crypto PRIVATE OpenSSL::SSL)
else()
  message(STATUS "Elogpp will be compiled without SSL support !!!")
  target_include_directories(Connector PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
endif()


add_library(elogpp STATIC ${ELOGCPP_SOURCES})
add_dependencies(elogpp jsoncpp Connector)
target_include_directories(elogpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(elogpp INTERFACE jsoncpp PUBLIC Connector)
install(FILES ${ELOGCPP_PUBLIC_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/elogpp")

install(TARGETS elogpp ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
