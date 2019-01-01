cmake_minimum_required(VERSION 3.10)
project(modeltest VERSION 1.1.1)

set(PRODUCTNAME "modeltest")
set(DESCRIPTION "modeltest")
set(COMPANY     "RTPTechGroup")
set(COPYRIGHT   "Copyright (C) RTPTechGroup")

include("../shared.cmake")

find_package(Qt5Core)
target_link_libraries(${PROJECT_NAME} Qt5::Core)

find_package(Qt5Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

find_package(Qt5Test)
target_link_libraries(${PROJECT_NAME} Qt5::Test)
