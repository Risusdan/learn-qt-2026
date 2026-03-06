# Week 14 — Cross-Platform & Deployment
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

# --- Platform properties ---------------------------------------------------
set_target_properties(DevConsole PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --- Install rules ----------------------------------------------------------
install(TARGETS DevConsole
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --- Deployment (Qt 6.3+) ---------------------------------------------------
# qt_generate_deploy_app_script generates a platform-correct deploy script
# that calls macdeployqt, windeployqt, or equivalent automatically.
if(QT_VERSION VERSION_GREATER_EQUAL "6.3")
    qt_generate_deploy_app_script(
        TARGET DevConsole
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
