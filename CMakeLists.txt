cmake_minimum_required(VERSION 3.30)
project(PlanDone)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
    Core
    Widgets
REQUIRED)

add_executable(PlanDone
    main.cpp

    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
)
target_link_libraries(PlanDone
    Qt::Core
    Qt::Widgets
)

