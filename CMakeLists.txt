set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

cmake_minimum_required(VERSION 3.30)
project(PlanDone)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(NeLib REQUIRED)

find_package(Qt6 COMPONENTS
    Core
    Widgets
REQUIRED)


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
    c++experimental
)

