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
    add_compile_options(/Zc:__cplusplus)
    set(close_console_flag WIN32)
else()
    list(APPEND CMAKE_PREFIX_PATH
            "S:/Libs/QT/6.9.0/mingw_64/"
    )
    set(close_console_flag WIN32)
endif()

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
message(STATUS ${Qt6_DIR})

# Function to copy Qt DLLs
function(copy_qt_dlls target)
    if(WIN32)
        # Get the Qt6 directory path
        get_target_property(Qt6Core_location Qt6::Core LOCATION)
        get_filename_component(Qt6_dir "${Qt6Core_location}" DIRECTORY)

        # Determine the DLLs needed based on the components we're using
        set(qt_dlls
                Qt6Core.dll
                Qt6Widgets.dll
                Qt6Gui.dll
        )

        # For each DLL, add a copy command
        foreach(dll ${qt_dlls})
            add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${Qt6_dir}/${dll}"
                    $<TARGET_FILE_DIR:${target}>
                    COMMENT "Copying ${dll} to output directory"
            )
        endforeach()

        # Copy platform plugin (needed for Qt applications)
        set(platform_plugin_dir "${Qt6_dir}/../plugins/platforms")
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${platform_plugin_dir}"
                "$<TARGET_FILE_DIR:${target}>/platforms"
                COMMENT "Copying Qt platform plugins"
        )
    endif()
endfunction()

add_library(Modules)
target_sources(Modules
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        User.cppm
        Plan.cppm
        Calendar.cppm
        Time.cppm
)
target_link_libraries(Modules PRIVATE
        Qt6::Core
        Qt6::Widgets
)

add_executable(PlanDone
        ${close_console_flag}
        main.cpp

        tools.cpp

        MainWindow/MainWindow.cpp
        MainWindow/MainWindow.ui

        AddWindow/AddWindow.cpp
        AddWindow/AddWindow.ui
)

target_include_directories(PlanDone PRIVATE
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(PlanDone PRIVATE
        Qt6::Core
        Qt6::Widgets
        Modules
)

# Copy Qt DLLs for the executable
copy_qt_dlls(PlanDone)