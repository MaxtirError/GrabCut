cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(GrabCut VERSION 1.0)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
set(CMAKE_PREFIX_PATH "D:/app/Qt5/5.15.2/msvc2019_64")
Ubpa_InitUCMake()

Ubpa_InitProject()

Ubpa_QtInit(COMPONENTS Core Gui Widgets)

find_package(OpenCV REQUIRED)

Ubpa_AddSubDirsRec(src)