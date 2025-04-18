cmake_minimum_required(VERSION 3.30)
project(PlanDone)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(Qt6_DIR "D:\\Libs\\QT\\6.9.0\\mingw_64\\lib\\cmake\\Qt6")
endif ()


find_package(Qt6 COMPONENTS
    Core
    Widgets
REQUIRED)

find_package(NeLib REQUIRED)

add_library(Modules)

target_sources(Modules
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    Plan.cppm
    Calendar.cppm
)

add_executable(PlanDone
    main.cpp

    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
)

target_link_libraries(Modules
    NeLib
)
target_link_libraries(PlanDone
    Qt::Core
    Qt::Widgets
    Modules
    NeLib
)

