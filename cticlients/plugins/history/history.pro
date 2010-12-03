include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = history_*.ts

TARGET      = $$qtLibraryTarget(historyplugin)

RESOURCES = res.qrc
