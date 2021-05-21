cmake_minimum_required(VERSION 2.8.8)
project(eagle_x)

SET(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.12.10\\5.12.10\\msvc2017\\")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

## Use debug logic
add_compile_options(-DDEBUG_BUILD)

## Executable name
set(TARGET EagleX)

## Qt5 required packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Location REQUIRED)
find_package(Qt5QuickWidgets REQUIRED)

set(INC ${CMAKE_SOURCE_DIR}/inc)
set(SRC ${CMAKE_SOURCE_DIR}/src)
set(QRC ${CMAKE_SOURCE_DIR}/qrc)

## Include main files
include_directories(
    ${INC}
    ${INC}/core
    ${INC}/core/messages
    ${INC}/gui
    ${INC}/gui/generated
    ${INC}/helpers
    ${INC}/helpers/math
    ${INC}/msgs
)

## Include Qt5
include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

## Source files
set(SOURCE_EXE
    ${SRC}/core/Core.cpp
    ${SRC}/core/Configurator.cpp
    ${SRC}/core/Communicator.cpp
    ${SRC}/core/DownloadManager.cpp
    ${SRC}/gui/UserInterface.cpp
    ${SRC}/gui/PlannerWidget.cpp
    ${SRC}/helpers/TimeUtils.cpp
    ${SRC}/helpers/Log.cpp
    ${SRC}/helpers/Crc32.cpp
    ${SRC}/helpers/math/MathUtils.cpp
    ${SRC}/helpers/math/LinePath2D.cpp
    ${SRC}/main.cpp
)

## Generate MOC files
qt5_wrap_cpp(SOURCE_EXE
    ${INC}/gui/UserInterface.h
    ${INC}/gui/PlannerWidget.h
)

## Qt5 resource files
qt5_add_resources(SOURCE_EXE ${QRC}/qml.qrc)

add_executable(${TARGET} ${SOURCE_EXE})

qt5_use_modules(${TARGET} Core Widgets Location QuickWidgets)
