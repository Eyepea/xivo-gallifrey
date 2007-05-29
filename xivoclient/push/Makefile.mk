# $Revision$
# $Date$

include $(WRK_DIR)/../autobuild.mk

DEB_PKG="pf-xivo-cti-pushagi"
DEB_TB_DEPS="python2.4-dev upx-ucl"
DEB_TAR_EXTRA_OPTIONS="--exclude=*.py"

FREEZEPATH?=$(WRK_DIR)/../tools/python-freeze/

default:

prepare-tarball::
	@${FREEZEPATH}/local_freeze.py ${FREEZEPATH}/freeze.py generefiche.py,xivo_push.py

clean-tarball::
	@find . ${FREEZEPATH} $(WRK_DIR)/../lib-python/ -name "*.pyc" -exec rm -f {} \;
	@rm -f xivo_push

