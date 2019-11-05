#option
option(BUILD_TEST "Build the test executable" ON)

# find files
file(GLOB ELOGCPP_PUBLIC_HEADERS include/*.h)
file(GLOB ELOGCPP_SOURCES src/*.cpp)

install(FILES ${ELOGCPP_PUBLIC_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/elogpp")
ADD_LIBRARY( elogcpp STATIC ${ELOGCPP_SOURCES})
add_dependencies(elogcpp jsoncpp pstreams)
link_libraries(elogcpp jsoncpp)
target_include_directories(elogcpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_INSTALL_PREFIX}/include/" )
#TARGET_LINK_LIBRARIES(elogcpp jsoncpp)
INSTALL( TARGETS elogcpp ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

if(BUILD_TEST)
    ADD_EXECUTABLE( test_elogcpp test_elogcpp.cpp)
    target_include_directories(test_elogcpp  PUBLIC "${CMAKE_INSTALL_PREFIX}/include/")
    target_link_libraries(test_elogcpp elogcpp)
    install(TARGETS test_elogcpp  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
