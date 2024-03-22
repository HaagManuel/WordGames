cmake_minimum_required(VERSION 3.0.0)
project(amq VERSION 0.1.0 LANGUAGES C CXX)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -pg")

string(REPLACE "-pg" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

message(STATUS "CMAKE_CXX_FLAGS:                ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG:          ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE:        ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

include(CPack)
include(CTest)
include_directories(.)

### Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(main main.cpp)

add_executable(tests tests.cpp)
target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)