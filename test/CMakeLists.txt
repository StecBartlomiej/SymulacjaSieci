# == Unit testing using Google Testing Framework ==

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCES_FILES_TESTS
        test/test_package.cpp
        test/test_storage_types.cpp
        test/test_nodes.cpp
        test/test_Factory.cpp
        test/test_factory_io.cpp
        test/test_reports.cpp
        )

add_executable(${PROJECT_NAME}_test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

target_compile_definitions(${PROJECT_NAME}_test PUBLIC EXERCISE_ID=REPORTING)

target_link_libraries(
        ${PROJECT_NAME}_test
        GTest::gtest_main
        GTest::gmock
)

target_include_directories(${PROJECT_NAME}_test PUBLIC
        mocks)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)