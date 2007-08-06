# -*- mode: makefile; coding: utf-8 -*-

remove-tarball::
	@rm -f $(DEB_DESTDIR)/$(DEB_PKG)_*.orig.tar.gz

do-tarball::
	tar zcf $(DEB_DESTDIR)/$(DEB_PKG)_$(XIVO_REV).orig.tar.gz -C ${TARBALL_DIR} $(DEB_TAR_COMMON_OPTIONS) $(DEB_TAR_EXTRA_OPTIONS) .

