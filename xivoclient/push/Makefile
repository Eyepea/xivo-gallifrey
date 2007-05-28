# $Revision$
# $Date$

FREEZEPATH?=../../tools/python-freeze/
MAINREV=`cat ../../VERSION`
SVNREV=`svn info | grep "Last Changed Rev" | sed "s/.*: //"`
XIVO_REV=${MAINREV}~svn${SVNREV}
DEB_PKG="pf-xivo-cti-pushagi"

default:

frozen:
	@${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py generefiche.py,xivo_push.py

tarball: frozen
	@rm -f ${DEB_PKG}_*.orig.tar.gz
	@tar zcf ${DEB_PKG}_${XIVO_REV}.orig.tar.gz --exclude .svn xivo_push xivo_push.conf
