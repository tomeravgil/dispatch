# 1. Minimum version requirement
cmake_minimum_required(VERSION 3.20)

# 2. Project name
project(dispatch_project)

# 3. Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Find the CURL package
find_package(CURL REQUIRED)

# 5. Create a core library containing the application logic (excluding main.cpp)
add_library(dispatch_core STATIC
        src/CommandProcessor.cpp
        src/CommandProcessor.h
        src/ConfigManager.cpp
        src/ConfigManager.h
        src/WebhookPublisher.cpp
        src/WebhookPublisher.h
)
target_link_libraries(dispatch_core PRIVATE CURL::libcurl)
target_include_directories(dispatch_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 6. Create the main executable and link the core library
add_executable(dispatch src/main.cpp)
target_link_libraries(dispatch PRIVATE dispatch_core)

# 7. Add Testing Support using GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add test executable
add_executable(dispatch_test 
    tests/test_command_processor.cpp
    tests/test_config_manager.cpp
    tests/test_webhook_publisher.cpp
)
target_link_libraries(dispatch_test PRIVATE dispatch_core gtest_main)

include(GoogleTest)
gtest_discover_tests(dispatch_test)