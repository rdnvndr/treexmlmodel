include(../../manual.pri)
greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
}
LIBS += -ltreexmlmodel
CONFIG  += qt
HEADERS   = *.h
SOURCES   = *.cpp
RESOURCES = *.qrc
FORMS     = *.ui


