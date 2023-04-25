cmake_minimum_required(VERSION 3.24)
project(DataMaker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# 如果没有配置Qt环境变量
# 修改下面的设置为Qt路径

#

if(WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/mingw73_64/bin")
endif ()

find_package(Qt5 COMPONENTS
            Core
            Gui
            Widgets
        QUIET)

find_package(Qt6 COMPONENTS
            Core
            Gui
            Widgets
        QUIET)


add_executable(
        DataMaker
#        WIN32
        main.cpp
        src/views/MainWindow/mainwindow.cpp
        src/views/MainWindow/mainwindow.h
        src/views/MainWindow/mainwindow.ui
        src/libs/DataMaker.h
        src/libs/DataMakerFromText.h
)

if(NOT Qt6_FOUND)
    target_link_libraries(DataMaker
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
            )
else()
    target_link_libraries(DataMaker
            Qt6::Core
            Qt6::Gui
            Qt6::Widgets
            )
endif()


if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
