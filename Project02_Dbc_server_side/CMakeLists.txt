cmake_minimum_required(VERSION 3.19)
project(Project02_Dbc_server_side LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core WebSockets Protobuf)

qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/canframe.proto)

qt_add_protobuf(ProtoMessages
    GENERATE_PACKAGE_SUBFOLDERS
    GENERATE_ALIASES
    PROTO_FILES ${PROTO_FILES}
)

qt_add_executable(Project02_Dbc_server_side
    main.cpp
)

target_link_libraries(Project02_Dbc_server_side
    PRIVATE
    Qt::Core
    Qt::WebSockets
    Qt::Protobuf
    ProtoMessages

)

include(GNUInstallDirs)

install(TARGETS Project02_Dbc_server_side
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
