# Week 13 — Serial Monitor Advanced
cmake_minimum_required(VERSION 3.16)
project(DevConsole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

qt_add_executable(DevConsole
    main.cpp
    MainWindow.h       MainWindow.cpp
    LogViewer.h        LogViewer.cpp
    LogModel.h         LogModel.cpp
    LogFilterProxy.h   LogFilterProxy.cpp
    LogDelegate.h      LogDelegate.cpp
    CodeEditor.h       CodeEditor.cpp
    CppHighlighter.h   CppHighlighter.cpp
    TextEditorTab.h    TextEditorTab.cpp
    FileBrowser.h      FileBrowser.cpp
    FindReplaceDialog.h FindReplaceDialog.cpp
    SerialMonitorTab.h  SerialMonitorTab.cpp
    SerialWorker.h      SerialWorker.cpp
    HexViewer.h         HexViewer.cpp
    ProcessLauncher.h   ProcessLauncher.cpp
)

target_link_libraries(DevConsole PRIVATE Qt6::Widgets Qt6::SerialPort)
