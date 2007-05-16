# $Revision$
# $Date$

TMPREP=/tmp/frpython
FROZENNAME=xivo_daemon.py.fz

default: frozen

frozen:
	mkdir -p ${TMPREP}
	${FREEZEPATH}/freeze.py -o ${TMPREP} xivo_daemon.py
	make -C ${TMPREP}
	mv ${TMPREP}/xivo_daemon ${FROZENNAME}
	rm -rf ${TMPREP}

# doc : doxygen @ nanard

