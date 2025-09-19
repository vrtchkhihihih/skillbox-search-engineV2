include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: prevent overriding parent's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "MSVC: Use shared CRT" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(search_engine_tests basic_tests.cpp)
target_link_libraries(search_engine_tests PRIVATE searchlib GTest::gtest_main)
set_target_properties(search_engine_tests PROPERTIES FOLDER "tests")

include(GoogleTest)
gtest_discover_tests(search_engine_tests)
