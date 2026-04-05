cmake_minimum_required(VERSION 3.19)
project(Project03_DbcTool LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network WebSockets Protobuf)

qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/canframe.proto)

qt_add_protobuf(ProtoMessages
    GENERATE_PACKAGE_SUBFOLDERS
    GENERATE_ALIASES
    PROTO_FILES ${PROTO_FILES})

qt_add_executable(Project03_DbcTool
    WIN32 MACOSX_BUNDLE
    main.cpp
    dbctool.cpp
    dbctool.h
    dbctool.ui
    canframebuilder.h canframebuilder.cpp
    canwebsocketclient.h canwebsocketclient.cpp
    Constants.h
    dbcparser.h dbcparser.cpp
    jsonserializer.h jsonserializer.cpp
    logger.h logger.cpp
    rangevalidator.h rangevalidator.cpp
    signalinputpanel.h signalinputpanel.cpp
    tabmanager.h tabmanager.cpp
    utils.h utils.cpp
)

target_link_libraries(Project03_DbcTool
    PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Network
    Qt::WebSockets
    Qt::Protobuf
    ProtoMessages
)

target_include_directories(Project03_DbcTool PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

include(GNUInstallDirs)

install(TARGETS Project03_DbcTool
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Project03_DbcTool
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
