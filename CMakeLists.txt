cmake_minimum_required(VERSION 3.30)
project(PlanDone)

# 基础配置
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


if(MSVC)
    list(APPEND CMAKE_PREFIX_PATH 
        "S:/Libs/opencv/build"
        "S:/Libs/QT/6.9.0/msvc2022_64/"
	)
    add_subdirectory("S:/Projects/NeLib" NeLib)
    add_compile_options(/Zc:__cplusplus)
    set(close_console_flag WIN32)



else ()

endif()

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
message(STATUS ${Qt6_DIR})

add_library(Modules)
target_sources(Modules
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
		User.cppm
        Plan.cppm
        Calendar.cppm
)
target_link_libraries(Modules PRIVATE
	NeLib
    Qt6::Core
    Qt6::Widgets
)

add_executable(PlanDone
    # ${close_console_flag}
    main.cpp

    tools.cpp

    MainWindow/MainWindow.cpp
    MainWindow/MainWindow.ui

    AddWindow/AddWindow.cpp
    AddWindow/AddWindow.ui
)
target_include_directories(PlanDone
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)
target_link_libraries(PlanDone PRIVATE
    Qt6::Core
    Qt6::Widgets
    Modules
    NeLib
)

# 添加自动部署命令
get_target_property(Qt6_qmake_location Qt6::qmake IMPORTED_LOCATION)
get_filename_component(Qt6_bin_dir "${Qt6_qmake_location}" DIRECTORY)
find_program(WINDEPLOYQT_EXE windeployqt HINTS "${Qt6_bin_dir}")

if(WINDEPLOYQT_EXE AND MSVC)
    add_custom_command(TARGET PlanDone POST_BUILD
        COMMAND "${WINDEPLOYQT_EXE}"
            --$<IF:$<CONFIG:Release>,release,debug>
            --dir "$<TARGET_FILE_DIR:PlanDone>"
            --no-compiler-runtime
            "$<TARGET_FILE:PlanDone>"
        COMMENT "Auto-deploying Qt dependencies..."
    )
endif()