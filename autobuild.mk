#!/usr/bin/make -f
# -*- mode: makefile; coding: utf-8 -*-

# parameters
WRK_DIR ?= $(shell pwd)
TARBALL_DIR = .
DEB_DESTDIR ?= $(WRK_DIR)/..
DEB_TAR_COMMON_OPTIONS = --exclude .svn
DEB_TAR_EXTRA_OPTIONS ?=
DEB_TB_DEPS ?=
DEB_TB_SCRAMBLED ?= "0"
FORCE_UNSCRAMBLED ?=

# internal variables
MAINREV=$(shell cat $(WRK_DIR)/../VERSION)
SVNREV=$(shell svn info | grep "Last Changed Rev" | sed "s/.*: //")
XIVO_REV=$(MAINREV)~svn$(SVNREV)


tarball-pkg:
	@if [ -z "$(DEB_PKG_LIST)" ]; then \
		echo "$(DEB_PKG)"; \
	else \
		echo "$(DEB_PKG_LIST)"; \
	fi

tarball-dep:
	@echo "$(DEB_TB_DEPS)"

tarball-scrambled:
	@echo "$(DEB_TB_SCRAMBLED)"

tarball: prepare-tarball do-tarball clean-tarball

remove-tarball::

prepare-tarball:: remove-tarball

do-tarball::

clean-tarball::

