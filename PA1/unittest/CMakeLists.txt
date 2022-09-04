enable_testing()

include(${FM_3RD_PARTY_DIR}/doctest/doctest.cmake)

add_executable(basics basics.cpp)

target_include_directories(basics PUBLIC ${FM_3RD_PARTY_DIR}/doctest)

target_include_directories(basics PUBLIC ${PROJECT_SOURCE_DIR}/src)

doctest_discover_tests(basics)

