# $Revision$
# $Date$

TMPREP=/tmp/frpython
TMPLOCAL=localtmp
FREEZEPATH=/home/corentin/subversion/eyefar/trunk/python-freeze
FROZENNAME=xivo_push.py.fz
REV=389


default: frozen

frozen:
	mkdir -p ${TMPREP}
	${FREEZEPATH}/freeze.py -o ${TMPREP} xivo_push
	make -C ${TMPREP}
	mv ${TMPREP}/xivo_push.bin ${FROZENNAME}
	rm -rf ${TMPREP}

tgz:
	mkdir -p ${TMPLOCAL}
	cp xivo_push.conf ${FROZENNAME} ${TMPLOCAL}
	cd ${TMPLOCAL} && tar zcf /tmp/final.tgz . && cd ..
	mv /tmp/final.tgz .
	rm -rf ${TMPLOCAL}
