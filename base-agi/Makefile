# $Revision$
# $Date$

DESTDIR?=.

MAINREV=`cat ../VERSION`
SVNREV=`svn info | grep "Last Changed Rev" | sed "s/.*: //"`
XIVO_REV=${MAINREV}~svn${SVNREV}

tarball-dep:
	@echo ""

tarball:
	@rm -f ${DESTDIR}/pf-xivo-agi_*.orig.tar.gz
	@tar zcf ${DESTDIR}/pf-xivo-agi_${XIVO_REV}.orig.tar.gz agi-bin bin xivo_agi.conf

