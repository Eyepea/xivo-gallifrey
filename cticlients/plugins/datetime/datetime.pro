include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = datetime_*.ts

TARGET      = $$qtLibraryTarget(datetimeplugin)

RESOURCES = res.qrc
