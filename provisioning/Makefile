# $Revision$
# $Date$

DESTDIR?=.
FREEZEPATH?=../tools/python-freeze/

MAINREV=`cat ../VERSION`
SVNREV=`svn info | grep "Last Changed Rev" | sed "s/.*: //"`
XIVO_REV=${MAINREV}~svn${SVNREV}

default:

frozen:
	@PYTHONPATH=../lib-python ./gen_static_init.py
	@${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py provsup.py,autoprov.py
	@${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py provsup.py,initconfig.py

tarball: frozen
	@rm -f pf-xivo-provisioning_*.orig.tar.gz
	@tar zcf ${DESTDIR}/pf-xivo-provisioning_${XIVO_REV}.orig.tar.gz autoprov initconfig etc files Phones --exclude=.svn --exclude=Phones/Thomson.py --exclude=Phones/Snom.py --exclude=Phones/__init__.py

tarball-dep:
	@echo "python2.4-dev upx-ucl"
