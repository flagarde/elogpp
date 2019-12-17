#option
option(BUILD_TEST "Build the test executable" ON)

# find files
set(ELOGCPP_PUBLIC_HEADERS 
	include/ElogCommand.h  
	include/ElogConfig.h  
	include/ElogEntry.h  
	include/ElogManager.h  
	include/ElogMessage.h  
	include/ElogServer.h  
	include/ElogUser.h  
	include/Logbook.h 
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
)

add_library(elogcpp STATIC ${ELOGCPP_SOURCES} ${ELOGCPP_PUBLIC_HEADERS})
add_dependencies(elogcpp jsoncpp pstreams)
target_include_directories(elogcpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" PRIVATE "${CMAKE_INSTALL_PREFIX}/include/")
#link_libraries(elogcpp jsoncpp)
target_link_libraries(elogcpp INTERFACE jsoncpp)
install(FILES ${ELOGCPP_PUBLIC_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/elogpp")

#TARGET_LINK_LIBRARIES(elogcpp jsoncpp)
INSTALL( TARGETS elogcpp ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

if(BUILD_TEST)
    add_executable(test_elogcpp test_elogcpp.cpp)
    target_include_directories(test_elogcpp  PRIVATE "${CMAKE_INSTALL_PREFIX}/include/")
    target_link_libraries(test_elogcpp elogcpp)
    install(TARGETS test_elogcpp  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
