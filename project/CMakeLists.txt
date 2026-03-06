# Week 5 — File I/O & Dialogs
cmake_minimum_required(VERSION 3.16)
project(DevConsole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(DevConsole
    main.cpp
    MainWindow.h MainWindow.cpp
)

target_link_libraries(DevConsole PRIVATE Qt6::Widgets)
