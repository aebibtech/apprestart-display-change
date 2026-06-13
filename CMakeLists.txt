cmake_minimum_required(VERSION 3.10)
project(DisplayMonitor VERSION 0.1.2)

set(CMAKE_CXX_STANDARD 17)

add_executable(DisplayMonitor WIN32
    main.cpp 
    Logger.cpp 
    ProcessManager.cpp 
    ConfigManager.cpp
)

target_link_libraries(DisplayMonitor User32 Shell32)
