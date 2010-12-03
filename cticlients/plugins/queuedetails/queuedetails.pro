include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queuedetails_*.ts

TARGET      = $$qtLibraryTarget(queuedetailsplugin)

RESOURCES = res.qrc
