#!/usr/bin/env python

__version__ = "$Revision$ $Date$"

from distutils.core import setup

setup(name='xivo_daemon',
      version='1.0',
      description='XiVO CTI Server Daemon',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      packages=['xivo_ctiservers',
		'xivo_ctiservers.CommandSets'],
      data_files=[
		  ('/etc/pf-xivo/ctiservers', ['xivo_daemon.conf']),
		  ('/usr/sbin', ['xivo_daemon']),
		 ],
     )
