# $Revision$
# $Date$

TMPREP=/tmp/frpython
TMPLOCAL=localtmp
FREEZEPATH=/home/corentin/subversion/eyefar/trunk/python-freeze
FROZENNAME=xivo_daemon.py.fz
REV=389

default: frozen

frozen:
	mkdir -p ${TMPREP}
	${FREEZEPATH}/freeze.py -o ${TMPREP} xivo_daemon.py
	make -C ${TMPREP}
	mv ${TMPREP}/xivo_daemon ${FROZENNAME}
	rm -rf ${TMPREP}

tgz:
	mkdir -p ${TMPLOCAL}
	cp xivo_daemon.conf ${FROZENNAME} xivo_daemon ${TMPLOCAL}
	cd ${TMPLOCAL} && tar zcf /tmp/final.tgz . && cd ..
	mv /tmp/final.tgz .
	rm -rf ${TMPLOCAL}

# doc : doxygen @ nanard

