# $Revision$
# $Date$

default:

frozen:
	${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py xivo_ami.py,xivo_sip.py,xivo_daemon.py

tarball: frozen
	tar zcf pf-xivo-cti-server.orig.tar.gz xivo_daemon xivo_daemon.conf

