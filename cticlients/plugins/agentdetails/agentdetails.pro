include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agentdetails_fr.ts

TARGET      = $$qtLibraryTarget(agentdetailsplugin)

RESOURCES = res.qrc
