include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agents_*.ts

TARGET      = $$qtLibraryTarget(agentsplugin)

RESOURCES = res.qrc
