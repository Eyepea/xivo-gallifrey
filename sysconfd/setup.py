#!/usr/bin/env python

__version__ = "$Revision$ $Date$"

from distutils.core import setup

setup(name='xivo_sysconfd',
      version='1.1',
      description='XIVO sysconf daemon',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      packages=['xivo_sysconf', 'xivo_sysconf.modules'],
      data_files=[
                  ('/usr/sbin', ['sysconfd']),
                 ],
     )

