include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agentsnext_*.ts

TARGET      = $$qtLibraryTarget(agentsnextplugin)

RESOURCES = res.qrc
