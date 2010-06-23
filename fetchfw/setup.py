#!/usr/bin/env python


__version__ = "$Revision$ $Date$"

from distutils.core import setup

setup(name='xivo_fetchfw',
      version='1.0',
      description='XIVO tool to fetch and install phone and card firmwares',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      packages=['xivo_fetchfw', 'xivo_fetchfw.brands'],
      data_files=[
                  ('/etc/pf-xivo', ['etc/fetchfw.conf']),
                  ('/usr/sbin', ['sbin/xivo_fetchfw']),
                  ('/usr/share/pf-xivo-fetchfw', ['data/firmwares.db']),
                 ],
     )
