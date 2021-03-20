include_guard(GLOBAL)

include(Messages)

if(NOT DEFINED OPENSSLCMAKE_REPOSITORY)
  set(OPENSSLCMAKE_REPOSITORY "https://gitlab.com/flagarde/openssl-cmake")
endif()

if(NOT DEFINED OPENSSLCMAKE_VERSION)
  set(OPENSSLCMAKE_VERSION "v2.0")
endif()

if(NOT DEFINED OPENSSL_VERSION)
  set(OPENSSL_VERSION "1.1.1j")  
endif()

if(NOT DEFINED OPENSSL_URL)
  set(OPENSSL_URL "https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz")
endif()

if(NOT DEFINED OPENSSL_URL_HASH)
  set(OPENSSL_URL_HASH "SHA256=aaf2fcb575cdf6491b98ab4829abf78a3dec8402b8b81efc8f23c00d443981bf")
endif()

if(USE_SYSTEM_OPENSSL)
  find_package(OpenSSL QUIET)
  if(OPENSSL_FOUND)
    find_package(OpenSSL)
    set(COMPILE_OPENSSL FALSE)
  else()
    set(COMPILE_OPENSSL TRUE)
    message(NOTE "OpenSSL not found ! OpenSSL will be compiled using ${OPENSSLCMAKE_REPOSITORY} version ${OPENSSLCMAKE_VERSION} !")
  endif()
else()
  set(COMPILE_OPENSSL TRUE)
  message(NOTE "OpenSSL will be compiled using ${OPENSSLCMAKE_REPOSITORY} version ${OPENSSLCMAKE_VERSION}.")
endif()

if(COMPILE_OPENSSL)
  include(CPM)
  cpm()
  declare_option(REPOSITORY OpenSSL-CMake OPTION OPENSSL_VERSION VALUE "${OPENSSL_VERSION}")
  declare_option(REPOSITORY OpenSSL-CMake OPTION WITH_APPS VALUE OFF)
  declare_option(REPOSITORY OpenSSL-CMake OPTION WITH_DOCS VALUE OFF)
  declare_option(REPOSITORY OpenSSL-CMake OPTION CMAKE_INSTALL_PREFIX VALUE ${CMAKE_INSTALL_PREFIX})
  print_options(REPOSITORY  OpenSSL-CMake)

  CPMAddPackage(NAME OpenSSL-CMake
                 GIT_REPOSITORY ${OPENSSLCMAKE_REPOSITORY}
                 GIT_TAG ${OPENSSLCMAKE_VERSION}
                 FETCHCONTENT_UPDATES_DISCONNECTED ${IS_OFFLINE}
                 OPTIONS "${OpenSSL-CMake_OPTIONS}")
endif()
