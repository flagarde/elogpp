add_executable(test_elogpp test_elogpp.cpp)
target_link_libraries(test_elogpp PUBLIC elogpp)
install(TARGETS test_elogpp ARCHIVE DESTINATION ${ARCHIVE_OUTPUT_DIR} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIR} LIBRARY DESTINATION ${LIBRARY_OUTPUT_DIR})

add_test(NAME test_elogpp COMMAND test_elogpp)
set_tests_properties(test_elogpp PROPERTIES ENVIRONMENT "ELOG_CONFIG_PATH=${CMAKE_CURRENT_SOURCE_DIR}/confs;ELOG_CONFIG_FILE=Elog.json")


