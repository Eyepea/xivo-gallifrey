# $Revision$
# $Date$

DESTDIR?=.

MAINREV=`cat ../VERSION`
SVNREV=`svn info | grep "Last Changed Rev" | sed "s/.*: //"`
XIVO_REV=${MAINREV}~svn${SVNREV}
DEB_PKG="pf-xivo-phones-firmware"

tarball-dep:
	@echo ""

tarball:
	@rm -f ${DESTDIR}/${DEB_PKG}_*.orig.tar.gz
	@tar zcf ${DESTDIR}/${DEB_PKG}_${XIVO_REV}.orig.tar.gz --exclude .svn Aastra Cisco Linksys Polycom Snom Swissvoice Thomson

