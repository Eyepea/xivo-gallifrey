# $Revision$
# $Date$

TMPREP=/tmp/frpython
FREEZEPATH=../../../eyefar/trunk/python-freeze
FROZENEXT=py.fz

default: frozen

frozen:
	mkdir -p ${TMPREP}
	${FREEZEPATH}/freeze.py -o ${TMPREP} autoprov.py
	make -C ${TMPREP}
	mv ${TMPREP}/autoprov autoprov.${FROZENEXT}
	rm -rf ${TMPREP}

	mkdir -p ${TMPREP}
	${FREEZEPATH}/freeze.py -o ${TMPREP} initconfig.py
	make -C ${TMPREP}
	mv ${TMPREP}/initconfig initconfig.${FROZENEXT}
	rm -rf ${TMPREP}

