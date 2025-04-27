cmake_minimum_required(VERSION 3.30)
project(PlanDone)

# 基础配置
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Windows 特定设置
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND CMAKE_PREFIX_PATH 
        "S:/Projects/NeLib/out/install/x64-Debug"
        "S:/Libs/opencv/build"
        "S:/Libs/QT/6.9.0/msvc2022_64"
	)
    link_directories("S:/Libs/QT/6.9.0/msvc2022_64/bin")
endif()

if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive- /utf-8 -D_UTF8)
    set(close_console_flag WIN32)
endif()

find_package(NeLib REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
message(STATUS "path is : " ${CMAKE_PREFIX_PATH})
message(STATUS "qt6 dir is : ${Qt6_DIR}")

add_library(Modules)
target_sources(Modules
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
        Plan.cppm
        Calendar.cppm
)
target_link_libraries(Modules PRIVATE NeLib)

add_executable(PlanDone
    ${close_console_flag}
    main.cpp
    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
)

target_link_libraries(PlanDone PRIVATE
    Qt6::Core
    Qt6::Widgets
    Modules
    NeLib
)