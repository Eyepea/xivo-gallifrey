######################################################################
#
# $Revision$
# $Date$
#

COMMONDIR = ../common
JSON_MJSON_DIR = ../json_mjson
OUTLOOK_DIR = ../3rdparty_outlook

_XIVOVER_ = 0.4
_SVNVER_ = 1
include(versions.pro)
XIVOVER = '\\"$${_XIVOVER_}\\"'
SVNVER  = '\\"$${_SVNVER_}\\"'
message('XIVO version:' $${XIVOVER})
message(' svn version:' $${SVNVER})
DEFINES += XIVOVER=\"$${XIVOVER}\"
DEFINES += SVNVER=\"$${SVNVER}\"

TEMPLATE = app
TARGET = 
DEPENDPATH += .
win32 {
	INCLUDEPATH += . $${COMMONDIR} $${JSON_MJSON_DIR} $${OUTLOOK_DIR}
} else {
	INCLUDEPATH += . $${COMMONDIR} $${JSON_MJSON_DIR}
}
CONFIG -= debug
CONFIG += static
CONFIG += uitools
CONFIG += x86 ppc

# Input
HEADERS += mainwidget.h $${COMMONDIR}/*.h
HEADERS += $${JSON_MJSON_DIR}/json.h

SOURCES += main.cpp mainwidget.cpp $${COMMONDIR}/*.cpp
SOURCES += $${JSON_MJSON_DIR}/json.c

win32 {
        DEFINES += USE_OUTLOOK=1
        LIBS += -lole32 -loleaut32 -luuid
        HEADERS += $${OUTLOOK_DIR}/*.h
        SOURCES += $${OUTLOOK_DIR}/*.cpp
}

QT += network
QT += xml
RESOURCES += appli.qrc
TRANSLATIONS = xivoclient_fr.ts qt_fr.ts
# TRANSLATIONS = xivoclient_fr.ts $$[QT_INSTALL_PREFIX]/translations/qt_fr.ts
RC_FILE = appli.rc
