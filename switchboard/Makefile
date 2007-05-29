# $Revision$
# $Date$

export

WRK_DIR ?= $(shell pwd)

default:

tarball-dep:
	# return concatenated tarball deps for each module
	@$(MAKE) -C daemon tarball-dep

tarball:
	# run each submodule tarball build
	@$(MAKE) -C daemon tarball

