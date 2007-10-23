#!/usr/bin/env python

__version__ = "$Revision$ $Date$"

from distutils.core import setup

setup(name='xivo',
      version='1.0',
      description='These are useful python libraries to be used with XIVO code',
      author='Guillaume Knispel',
      author_email='gknispel@proformatique.com',
      url='http://xivo.fr/',
      packages=['xivo', 'xivo.BackSQL'],
     )
