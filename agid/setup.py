#!/usr/bin/env python

__version__ = "$Revision: 2564 $ $Date: 2008-03-07 19:09:56 +0100 (Fri, 07 Mar 2008) $"

from distutils.core import setup

setup(name='xivo_agid',
      version='1.0',
      description='XIVO AGI Daemon',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      packages=['xivo_agid', 'xivo_agid.modules'],
      data_files=[
		  ('/usr/sbin', ['sbin/agid']),
		 ],
     )
