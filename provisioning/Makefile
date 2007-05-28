# $Revision$
# $Date$

DESTDIR?=.
FREEZEPATH?=../tools/python-freeze/

MAINREV=`cat ../VERSION`
SVNREV=`svn info | grep "Last Changed Rev" | sed "s/.*: //"`
XIVO_REV=${MAINREV}~svn${SVNREV}
DEB_PKG="pf-xivo-provisioning"

default:

frozen:
	@echo "from Phones import *" | PYTHONPATH=../lib-python/ /usr/bin/python
	@echo "import provsup" | PYTHONPATH=../lib-python/ /usr/bin/python
	@${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py provsup.py,autoprov.py
	@cp initconfig.py initconfig

tarball: frozen
	@rm -f ${DEB_PKG}_*.orig.tar.gz
	@tar zcf ${DESTDIR}/${DEB_PKG}_${XIVO_REV}.orig.tar.gz --exclude .svn autoprov initconfig provsup.pyc etc files Phones --exclude=.svn --exclude=Phones/Thomson.py --exclude=Phones/Snom.py --exclude=Phones/__init__.py

tarball-dep:
	@echo "python2.4-dev upx-ucl"

clean:
	@rm -f *.pyc Phones/*.pyc autoprov initconfig ${DEB_PKG}_*.orig.tar.gz
