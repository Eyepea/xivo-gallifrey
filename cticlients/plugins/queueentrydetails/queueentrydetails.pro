include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queueentrydetails_*.ts

TARGET      = $$qtLibraryTarget(queueentrydetailsplugin)

RESOURCES = res.qrc
