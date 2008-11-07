######################################################################
#
# $Revision$
# $Date$
#

COMMONDIR = ../common
JSON_JSONQT_DIR = ../3rdparty/json_jsonqt
OUTLOOK_DIR = ../3rdparty/outlook

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
INCLUDEPATH += . $${COMMONDIR} $${JSON_JSONQT_DIR}

CONFIG -= debug
CONFIG += static
CONFIG += uitools
CONFIG += x86 ppc

# Input
HEADERS += mainwidget.h $${COMMONDIR}/*.h
HEADERS += $${JSON_JSONQT_DIR}/*.h

SOURCES += main.cpp mainwidget.cpp $${COMMONDIR}/*.cpp
SOURCES += $${JSON_JSONQT_DIR}/*.cpp

win32 {
	INCLUDEPATH += $${OUTLOOK_DIR}
        DEFINES += USE_OUTLOOK=1
        DEFINES += MAKE_JSONQT_LIB
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
