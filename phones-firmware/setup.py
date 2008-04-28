#!/usr/bin/env python

__version__ = "$Revision: 2564 $ $Date: 2008-03-07 19:09:56 +0100 (Fri, 07 Mar 2008) $"

from distutils.core import setup

setup(name='xivo_fetchfw',
      version='1.0',
      description='XIVO tool to fetch and install phone firmwares',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      packages=['xivo_fetchfw'],
      data_files=[
		  ('/etc/pf-xivo', ['etc/fetchfw.conf']),
		  ('/usr/sbin', ['sbin/xivo_fetchfw']),
		  ('/usr/share/pf-xivo-phones-firmware', ['data/firmwares.db']),
		 ],
     )
