# $Revision$
# $Date$

FREEZEPATH?=../../tools/python-freeze/
MAINREV=`cat ../../VERSION`
SVNREV=`svn info | grep "Last Changed Rev" | sed "s/.*: //"`
XIVO_REV=${MAINREV}~svn${SVNREV}
DEB_PKG="pf-xivo-cti-server"

default:

frozen:
	@${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py xivo_ami.py,xivo_sip.py,xivo_daemon.py

tarball: frozen
	@rm -f ${DEB_PKG}_*.orig.tar.gz
	@tar zcf ${DEB_PKG}_${XIVO_REV}.orig.tar.gz --exclude .svn xivo_daemon xivo_daemon.conf

