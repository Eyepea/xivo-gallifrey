include(../plugins-global.pri)
include(../../qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = switchboard_fr.ts

TARGET      = $$qtLibraryTarget(switchboardplugin)

RESOURCES = res.qrc
