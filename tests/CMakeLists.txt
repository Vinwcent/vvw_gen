find_package(GTest REQUIRED)

enable_testing()
include(GoogleTest)
set(TESTS_FILES)
list(APPEND TESTS_FILES
	priority_event_queue_test.cpp
	function_wrappers_test.cpp
	args_storage_test.cpp
	observer_notifier_test.cpp
	thread_test.cpp
	segregated_handle_pool_test.cpp
)

set(TESTS_EXECUTABLE tests)
add_executable(${TESTS_EXECUTABLE} ${TESTS_FILES})
target_include_directories(${TESTS_EXECUTABLE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	${GTEST_INCLUDE_DIRS})
target_link_libraries(${TESTS_EXECUTABLE} vvw_gen::vvw_gen GTest::gtest_main GTest::gtest)
gtest_discover_tests(
	${TESTS_EXECUTABLE}
)
