include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = history_fr.ts

TARGET      = $$qtLibraryTarget(historyplugin)

RESOURCES = res.qrc
