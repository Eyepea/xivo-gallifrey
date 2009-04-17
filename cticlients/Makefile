# $Revision$
# $Date$

QMAKE?=qmake
LRELEASE?=lrelease
UPXRUN?=upx
ECHO?=/bin/echo
JOPT?=-j1
DOXYGEN?=doxygen

# WIN32 targets only
MAKENSIS=/cygdrive/c/Program\ Files/NSIS/makensis.exe
UPXWIN?=/cygdrive/c/upx303w/upx.exe

# Versions Definitions
-include versions.mak
-include uname.mak

default: help

help:
	@echo
	@echo "Please choose your target according to the OS you are using :"
	@echo " -- to build Linux binaries or debian packages :"
	@echo "          make all-linux "
	@echo "          make all-debian "
	@echo " -- to build Windows binaries (application + NSIS setup) :"
	@echo "          make all-win32 "
	@echo " -- to build Mac OS X binaries (Universal Binary applications + .dmg packages) :"
	@echo "          make all-macos "
	@echo ""
	@echo "You can also build docs using doxygen :"
	@echo "          make doc"
	@echo "Thanks."
	@${ECHO} -n "_UNAME_ = " > uname.mak
	@uname -s | sed "s/Linux/linux/;s/CYGWIN.*/win32/;s/Darwin/macos/" >> uname.mak

all:
	@echo "Will compile for target ${_UNAME_} : all-${_UNAME_}"
	@make all-${_UNAME_}

# LINUX targets
# kind of dirtier than "all-linux: versions-xivoclient linux-xivoclient"
# but allows the 'include versions.mak' to be reloaded once it has been set
all-linux:
	@make versions-xivoclient
	@make linux-xivoclient
	@make stripandpack-xivoclient

distclean:
	@make clean-xivoclient

clean-%:
	@make -C $* distclean || true

linux-%:
	@cd $* && ${QMAKE} && ${LRELEASE} $*_fr.ts qt_fr.ts && make ${JOPT}

stripandpack-%:
	@strip $*/$* || true
	@${UPXRUN} $*/$* || true

all-win32:
	@make -s versions-xivoclient
	@echo "   (under Qt prompt) :"
	@pwd | sed "s#/#\\\\#g;s#^#      cd C:\\\\cygwin#"
	@echo "      make win32-xivoclient"
	@echo "   (back here under Cygwin) :"
	@echo "      make win32pack-xivoclient"

# to be executed under a bash/cygwin-like terminal
versions-%:
	@${ECHO} -n "version (before update) : " && make -s displayversions
	@svn up
	@touch common/xivoconsts.h $*/mainwidget.cpp
	@rm -f $*/versions.pro
	@${ECHO} -n "_SVNVER_ = " >> $*/versions.pro
	@LANG=C svn info | sed -n "/Last Changed Rev/s/.*: //p" >> $*/versions.pro
	@grep -h "VER_ =" $*/*.pro | sort -r | head -2 > versions.mak
	@${ECHO} -n "_DATEBUILD_ = " >> $*/versions.pro
	@date +%Y-%m-%dT%H:%M:%S >> $*/versions.pro
	@${ECHO} -n "version (after update) : " && make -s displayversions

displayversions:
	@${ECHO} ${_XIVOVER_}-${_SVNVER_}

# to be used with care
forceversions-%:
	@${ECHO} -n "version (before force) : " && make -s displayversions
	@touch common/xivoconsts.h $*/mainwidget.cpp
	@rm -f $*/versions.pro
	@${ECHO} "_SVNVER_ =" ${FORCEVERSION} >> $*/versions.pro
	@grep -h "VER_ =" $*/*.pro | sort -r | head -2 > versions.mak
	@${ECHO} -n "version (after force) : " && make -s displayversions

# to be executed under a mingw/dos-like terminal
win32-%:
	@cd $* && ${QMAKE} $*.pro && ${LRELEASE} $*_fr.ts qt_fr.ts && make -f Makefile.Release

# to be executed under a bash/cygwin-like terminal
win32pack-%:
	@${UPXWIN} $*/release/$*.exe || true
	@${MAKENSIS} delivery/$*.nsi
	@mv delivery/$*-setup-win32.exe $*-setup-${_XIVOVER_}-${_SVNVER_}-win32.exe

win32packdyn-%:
	@${UPXWIN} $*/release/$*.exe || true
	@${MAKENSIS} delivery/$*4.5.nsi
	@mv delivery/$*-setup-win32.exe $*-setup-${_XIVOVER_}-${_SVNVER_}-win32.exe

# MACOS targets
# (man hdiutil) "-format UDBZ" works only >= 10.4, thus "-format UDZO"
# export UPXRUN=/Users/proformatique/upx-3.01-src/src/upx.out

all-macos:
	@make versions-xivoclient
	@make macos-xivoclient

macos-%:
	@cd $* && ${QMAKE} $*.pro -o Makefile && ${LRELEASE} $*_fr.ts qt_fr.ts && make ${JOPT}
	@strip $*/$*.app/Contents/MacOS/$*
	@${UPXRUN} $*/$*.app/Contents/MacOS/$*
	@mkdir -p $*/$*.app/Contents/Resources/French.lproj
	@hdiutil create $*-${_XIVOVER_}-${_SVNVER_}.dmg -srcfolder $*/$*.app -format UDZO


# DEBIAN targets

all-debian : debian-xivoclient debian-switchboard

debian-%:
	@echo "## Building Debian Package for : $*"
	@echo "## Changelog will be updated with ${NAME} <${DEBEMAIL}>"
	cd $* && dch -v ${XIVOVER}~svn${SVNVER}-1 "New upstream release."
	rm -rf pf-xivo-cti-$**
	mkdir pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}
	cp -rp $*/debian pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}/debian
	cd $* && tar zcf ../pf-xivo-cti-$*_${XIVOVER}~svn${SVNVER}.orig.tar.gz $*
	cp $*/$* pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}
	cd pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER} && \
		DEB_AUTO_UPDATE_DEBIAN_CONTROL=yes fakeroot debian/rules clean && \
		dpkg-buildpackage -us -uc -tc -rfakeroot

# build doc

doc:	common xivoclient
	${DOXYGEN} xivoclient/Doxyfile 
