if(USE_SYSTEM_OPENSSL)
  find_package(OpenSSL QUIET)
  if(OPENSSL_FOUND)
    set(COMPILE_OPENSSL FALSE)
  else()
    set(COMPILE_OPENSSL TRUE)
    message(STATUS "OpenSSL not found ! OpenSSL will be compiled !") 
  endif()
else()
  set(COMPILE_OPENSSL TRUE)
  message(STATUS "OpenSSL will be compiled") 
endif()

if(COMPILE_OPENSSL)
 if(NOT TARGET openssl_project)
    include(ExternalProject)  
    # ----- openssl_project package -----
    ExternalProject_Add(openssl_project
                        GIT_REPOSITORY ${OPENSSL_REPOSITORY}
                        GIT_TAG ${OPENSSL_TAG}
                        GIT_PROGRESS TRUE
                        GIT_SHALLOW TRUE
                        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD} -DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED} -DCMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE} -DWITH_APPS=OFF
                        PREFIX ${CMAKE_BINARY_DIR}/openssl_project
                        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
                        LOG_DOWNLOAD ON
                        UPDATE_DISCONNECTED ON
                       )
  endif()
        include(FindPackageHandleStandardArgs)
    # find_package(OpenSSL) may not have found it,
    # clear parent scope variables set to NOTFOUND
    foreach(suffix FOUND INCLUDE_DIR INCLUDE_DIRS CRYPTO_LIBRARY SSL_LIBRARY LIBRARY LIBRARIES VERSION)
      unset(OPENSSL_${suffix} PARENT_SCOPE)
    endforeach()
  
    foreach(lib ssl crypto)
      string(TOUPPER ${lib} libupper)
      set(archname "")
      if( MSVC )
        if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
          set(archname "64")
        elseif( CMAKE_SIZEOF_VOID_P EQUAL 4 )
          set(archname "32")
      endif()
    endif()
      set(libname lib${lib}${archname}${CMAKE_STATIC_LIBRARY_SUFFIX})
      set(OPENSSL_${libupper}_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${libname})
    endforeach()
        find_package(Threads)
    #FIXME Try to find the version alone
    set(OPENSSL_VERSION CACHE STRING "1.1.1.g" FORCE)
    set(OPENSSL_FOUND TRUE CACHE BOOL "" FORCE)
    set(OPENSSL_VERSION ${OPENSSL_VERSION} CACHE INTERNAL "" FORCE)
    set(OPENSSL_VERSION_STRING "${OPENSSL_VERSION}" CACHE INTERNAL "" FORCE)
    set(OPENSSL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE INTERNAL "" FORCE) # needed by Davix
    set(OPENSSL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE INTERNAL "" FORCE)
    set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_CRYPTO_LIBRARY}" CACHE INTERNAL "" FORCE)
    set(OPENSSL_SSL_LIBRARY "${OPENSSL_SSL_LIBRARY}" CACHE INTERNAL "" FORCE)
    set(OPENSSL_LIBRARIES "${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${CMAKE_DL_LIBS}" Threads::Threads CACHE INTERNAL "" FORCE)
    
    add_library(builtin_crypto INTERFACE)
    target_include_directories(builtin_crypto INTERFACE $<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>)
    target_link_libraries(builtin_crypto INTERFACE ${OPENSSL_CRYPTO_LIBRARY} INTERFACE ${CMAKE_DL_LIBS} INTERFACE Threads::Threads)
    add_dependencies(builtin_crypto openssl_project)
    add_library(OpenSSL::Crypto ALIAS builtin_crypto)
    
    add_library(builtin_ssl INTERFACE)
    target_include_directories(builtin_ssl INTERFACE $<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>)
    target_link_libraries(builtin_ssl INTERFACE ${OPENSSL_SSL_LIBRARY} INTERFACE OpenSSL::Crypto INTERFACE ${CMAKE_DL_LIBS} INTERFACE Threads::Threads)
    add_dependencies(builtin_ssl openssl_project)


    add_library(OpenSSL::SSL ALIAS builtin_ssl)
endif()
