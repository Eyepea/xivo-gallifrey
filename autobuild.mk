
# parameters
WRK_DIR ?= $(shell pwd)
DEB_DESTDIR ?= $(WRK_DIR)/..
DEB_TAR_EXTRA_OPTIONS ?=
TARBALL_DIR = .
DEB_TB_DEPS ?=
DEB_TB_SCRAMBLED ?= "0"
FORCE_UNSCRAMBLED ?=

# internal variables
MAINREV=$(shell cat $(WRK_DIR)/../VERSION)
SVNREV=$(shell svn info | grep "Last Changed Rev" | sed "s/.*: //")
XIVO_REV=$(MAINREV)~svn$(SVNREV)


tarball-dep:
	@echo "$(DEB_TB_DEPS)"

tarball-scrambled:
	@echo "$(DEB_TB_SCRAMBLED)"

tarball: prepare-tarball do-tarball clean-tarball

remove-tarball::
	@rm -f $(DEB_DESTDIR)/$(DEB_PKG)_*.orig.tar.gz

prepare-tarball:: remove-tarball

do-tarball::
	tar zcf $(DEB_DESTDIR)/$(DEB_PKG)_$(XIVO_REV).orig.tar.gz -C ${TARBALL_DIR} --exclude .svn $(DEB_TAR_EXTRA_OPTIONS) .

clean-tarball::

