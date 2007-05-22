# $Revision$
# $Date$

default:

frozen:
	${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py generefiche.py,xivo_push.py

tarball: frozen
	tar zcf pf-xivo-cti-pushagi.orig.tar.gz xivo_push xivo_push.conf

