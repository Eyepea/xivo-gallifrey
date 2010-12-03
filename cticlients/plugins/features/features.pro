include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = features_*.ts

TARGET      = $$qtLibraryTarget(featuresplugin)

RESOURCES = res.qrc
