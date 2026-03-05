# 1. Minimum version requirement
cmake_minimum_required(VERSION 3.20)

# 2. Project name
project(dispatch_project)

# 3. Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Find the CURL package
find_package(CURL REQUIRED)

# 5. Create the executable FIRST
# I changed "my_program" to "dispatch" to match your setup.sh expectations
add_executable(dispatch
        main.cpp
        CommandProcessor.cpp
        CommandProcessor.h
        ConfigManager.cpp
        ConfigManager.h
        WebhookPublisher.cpp
        WebhookPublisher.h
)

# 6. Link libraries AFTER the executable is defined
target_link_libraries(dispatch PRIVATE CURL::libcurl)