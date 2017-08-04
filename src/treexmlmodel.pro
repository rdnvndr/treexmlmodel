VERSION = 1.0.0
QMAKE_TARGET_COMPANY = "RTPTechGroup"
QMAKE_TARGET_PRODUCT = "TreeXmlModel Library"
QMAKE_TARGET_DESCRIPTION = "TreeXmlModel Library"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) RTPTechGroup"

include(../libs.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
}
QT += xml
DEFINES += XMLMODEL_LIBRARY
HEADERS   = *.h
SOURCES   = *.cpp
