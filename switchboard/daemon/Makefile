# $Revision$
# $Date$

default:

frozen:
	${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py xivo_ami.py,xivo_sip.py,xivo_daemon.py

tarball:
	${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py xivo_ami.py,xivo_sip.py,xivo_daemon.py
	tar zcf pf-xivo-cti-server.tgz xivo_daemon xivo_daemon.conf

