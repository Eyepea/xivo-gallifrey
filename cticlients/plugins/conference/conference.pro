include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = conference_*.ts

TARGET      = $$qtLibraryTarget(conferenceplugin)

RESOURCES = res.qrc
