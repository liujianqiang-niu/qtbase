cmake_minimum_required(VERSION 3.16)
project(qttablet LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/widgets/widgets/tablet")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_add_executable(qttablet
    main.cpp
    mainwindow.cpp mainwindow.h
    tabletapplication.cpp tabletapplication.h
    tabletcanvas.cpp tabletcanvas.h
)

set_target_properties(qttablet PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(qttablet PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

# Resources:
set(images_resource_files
    "images/cursor-airbrush.png"
    "images/cursor-eraser.png"
    "images/cursor-felt-marker.png"
    "images/cursor-pencil.png"
)

qt_add_resources(qttablet "images"
    PREFIX
        "/"
    FILES
        ${images_resource_files}
)

install(TARGETS qttablet
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
