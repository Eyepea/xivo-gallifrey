include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = calls_*.ts

TARGET      = $$qtLibraryTarget(callsplugin)

RESOURCES = res.qrc
