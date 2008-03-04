#!/usr/bin/env python

__version__ = "$Revision$ $Date$"

from distutils.core import setup

setup(name='xivo_provisioning',
      version='1.0',
      description='Xivo Provisioning daemon, AGI and modules',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      packages=['xivo_provisioning', 'xivo_provisioning.Phones'],
      scripts=['autoprov', 'initconfig'],
     )
