include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp

TARGET      = $$qtLibraryTarget(conf2plugin)

RESOURCES = res.qrc
