add_executable(test_elogpp test_elogpp.cpp)
target_link_libraries(test_elogpp PUBLIC elogpp)
install(TARGETS test_elogpp ARCHIVE DESTINATION ${ARCHIVE_OUTPUT_DIR} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIR} LIBRARY DESTINATION ${LIBRARY_OUTPUT_DIR})

# Check if elogd with default settings are set
execute_process(COMMAND "elog" "-h" "localhost" "-p" "8080" "-l" "demo" "-w" "last" OUTPUT_VARIABLE RESULT)

if(NOT RESULT STREQUAL "Cannot connect to host localhost, port 8080\n")
  add_test(NAME test_elogpp COMMAND test_elogpp)
  set_tests_properties(test_elogpp PROPERTIES ENVIRONMENT "ElogServerConfFile=${CMAKE_CURRENT_SOURCE_DIR}/confs/Elog.json")
endif()

