cmake_minimum_required(VERSION 3.16)
project(calculator LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/widgets/widgets/calculator")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_add_executable(calculator
    button.cpp button.h
    calculator.cpp calculator.h
    main.cpp
)

set_target_properties(calculator PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(calculator PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

install(TARGETS calculator
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
