include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = switchboard_*.ts

include(../../qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

TARGET      = $$qtLibraryTarget(switchboardplugin)

RESOURCES = res.qrc
