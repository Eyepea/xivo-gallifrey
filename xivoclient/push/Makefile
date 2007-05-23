# $Revision$
# $Date$

FREEZEPATH?=../../tools/python-freeze/
MAINREV=`cat ../../VERSION`
SVNREV=`svn info | grep "Last Changed Rev" | sed "s/.*: //"`
XIVO_REV=${MAINREV}~svn${SVNREV}

default:

frozen:
	@${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py generefiche.py,xivo_push.py

tarball: frozen
	@rm -f pf-xivo-cti-pushagi_*.orig.tar.gz
	@tar zcf pf-xivo-cti-pushagi_${XIVO_REV}.orig.tar.gz xivo_push xivo_push.conf
