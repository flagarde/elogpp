add_executable(test_elogpp test_elogpp.cpp)
target_link_libraries(test_elogpp PUBLIC elogpp)
install(TARGETS test_elogpp ARCHIVE DESTINATION ${ARCHIVE_OUTPUT_DIR} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIR} LIBRARY DESTINATION ${LIBRARY_OUTPUT_DIR})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/confs/Elog.json" "${CMAKE_CURRENT_BINARY_DIR}/confs/Elog.json" FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE @ONLY)
add_test(NAME test_elogpp COMMAND test_elogpp)
set_tests_properties(test_elogpp PROPERTIES ENVIRONMENT "ElogServerConfFile=${CMAKE_CURRENT_BINARY_DIR}/confs/Elog.json")


