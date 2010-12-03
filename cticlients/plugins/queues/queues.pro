include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queues_*.ts

TARGET      = $$qtLibraryTarget(queuesplugin)

RESOURCES = res.qrc
