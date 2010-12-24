#!/usr/bin/env python

__version__ = "$Revision$ $Date$"

from distutils.core import setup

setup(name='xivo_bnfos',
      version='1.0',
      description='Tool for managing beronet bero*fos device',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      scripts=['src/xivo_bnfos'],
      data_files=[('/etc/pf-xivo/', ['resource/etc/xivo_bnfos.conf',
                                     'resource/etc/xivo_bnfos.conf.example'])]
      )
