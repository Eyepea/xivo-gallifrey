# $Revision$
# $Date$

DESTDIR?=.

default:

tarball:
	@make -C daemon tarball
	@mv daemon/*.orig.tar.gz ${DESTDIR}

tarball-dep:
	@echo "python2.4-dev upx-ucl"
