set(ELOG_HEADERS 
  strlcpy.h
)

set(ELOG_SOURCES 
  strlcpy.cpp
  elog.cpp
)

add_library(crypt STATIC crypt.cpp)

add_executable(elog elog.cpp)
find_package(OpenSSL REQUIRED) 
if( OpenSSL_FOUND )
    target_include_directories(elog PRIVATE ${OPENSSL_INCLUDE_DIRS})
    target_compile_definitions(elog PRIVATE HAVE_SSL)
    target_link_libraries(elog PRIVATE crypt PRIVATE  OpenSSL::Crypto PRIVATE OpenSSL::SSL  )
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif()

install(TARGETS elog  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})

