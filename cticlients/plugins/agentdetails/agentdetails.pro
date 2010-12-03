include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agentdetails_*.ts

TARGET      = $$qtLibraryTarget(agentdetailsplugin)

RESOURCES = res.qrc
