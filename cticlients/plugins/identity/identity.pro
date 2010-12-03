include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = identity_*.ts

TARGET      = $$qtLibraryTarget(identityplugin)

RESOURCES = res.qrc
