add_executable(test_elogpp test_elogpp.cpp)
target_include_directories(test_elogpp  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(test_elogpp PUBLIC elogpp)
install(TARGETS test_elogpp ARCHIVE DESTINATION ${ARCHIVE_OUTPUT_DIR} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIR} LIBRARY DESTINATION ${LIBRARY_OUTPUT_DIR})
