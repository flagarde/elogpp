add_executable(test_elogpp test_elogpp.cpp)
target_include_directories(test_elogpp  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/" PRIVATE ${GENERATED_HEADER_PATH})
target_link_libraries(test_elogpp PRIVATE elogpp)
install(TARGETS test_elogpp  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
