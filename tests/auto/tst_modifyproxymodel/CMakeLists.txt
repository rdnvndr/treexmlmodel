cmake_minimum_required(VERSION 3.10)
project(tst_modifyproxymodel VERSION 1.0.0)

set(PRODUCTNAME "tst_modifyproxymodel")
set(DESCRIPTION "tst_modifyproxymodel")
set(COMPANY     "RTPTechGroup")
set(COPYRIGHT   "Copyright (C) RTPTechGroup")
set(LIBRARIES
    treexmlmodel
    modeltest
)
include(../auto.cmake)

find_package(Qt5Core)
target_link_libraries(${PROJECT_NAME} Qt5::Core)

find_package(Qt5Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

find_package(Qt5Test)
target_link_libraries(${PROJECT_NAME} Qt5::Test)
