configure_file(
  "include/ElogCommand.hpp.in"
  "${GENERATED_HEADER_PATH}/ElogCommand.hpp"
)

set(ELOGCPP_PUBLIC_HEADERS 
	"${GENERATED_HEADER_PATH}/ElogCommand.hpp"  
	include/ElogConfig.hpp  
	include/ElogEntry.hpp  
	include/ElogManager.hpp  
	include/ElogMessage.hpp  
	include/ElogServer.hpp  
	include/ElogUser.hpp 
	include/Logbook.hpp
	include/Connector.hpp
	include/Crypt.hpp
	include/Elog.hpp
)

set(ELOGCPP_SOURCES 
	src/ElogCommand.cpp  
	src/ElogConfig.cpp  
	src/ElogEntry.cpp  
	src/ElogManager.cpp  
	src/ElogMessage.cpp  
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
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif()


add_library(elogpp STATIC ${ELOGCPP_SOURCES})
add_dependencies(elogpp jsoncpp pstreams Connector)
target_include_directories(elogpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" PRIVATE "${CMAKE_INSTALL_PREFIX}/include/pstreams" PRIVATE ${GENERATED_HEADER_PATH})
target_link_libraries(elogpp INTERFACE jsoncpp PUBLIC Connector)
install(FILES ${ELOGCPP_PUBLIC_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/elogpp")

install(TARGETS elogpp ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
