# $Revision$
# $Date$

FREEZEPATH?=../../tools/python-freeze/
MAINREV=`cat ../../VERSION`
SVNREV=`svn info | grep "Last Changed Rev" | sed "s/.*: //"`
XIVO_REV=${MAINREV}~svn${SVNREV}

default:

frozen:
	@${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py xivo_ami.py,xivo_sip.py,xivo_daemon.py

tarball: frozen
	@tar zcf pf-xivo-cti-server_${XIVO_REV}.orig.tar.gz xivo_daemon xivo_daemon.conf

