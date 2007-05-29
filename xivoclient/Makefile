# $Revision$
# $Date$

export

WRK_DIR ?= $(shell pwd)

default:

tarball-dep:
	# return concatenated tarball deps for each module
	@$(MAKE) -C push tarball-dep

tarball:
	# run each submodule tarball build
	@$(MAKE) -C push tarball

