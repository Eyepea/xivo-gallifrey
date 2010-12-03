include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = parking_*.ts

TARGET      = $$qtLibraryTarget(parkingplugin)

RESOURCES = res.qrc
