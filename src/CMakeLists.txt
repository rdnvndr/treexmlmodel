cmake_minimum_required(VERSION 3.10)
project(treexmlmodel VERSION 1.0.0)

set(PRODUCTNAME "TreeXmlModel Library")
set(DESCRIPTION "TreeXmlModel Library")
set(COMPANY     "RTPTechGroup")
set(COPYRIGHT   "Copyright (C) RTPTechGroup")

include("${CMAKE_SOURCE_DIR}/cmake/libs.cmake")

target_compile_definitions(${PROJECT_NAME} PRIVATE -DXMLMODEL_LIBRARY)

find_package(Qt5Core)
target_link_libraries(${PROJECT_NAME} Qt5::Core)

find_package(Qt5Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

find_package(Qt5Xml)
target_link_libraries(${PROJECT_NAME} Qt5::Xml)
