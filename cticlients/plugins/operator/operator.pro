include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = operator_*.ts

TARGET      = $$qtLibraryTarget(operatorplugin)

RESOURCES = res.qrc
