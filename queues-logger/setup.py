#!/usr/bin/env python

__version__ = "$Revision$ $Date$"

from distutils.core import setup

setup(
      name='queue_logger',
      version='1.0',
      description='XIVO queue logger',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      packages=['xivo_queuelogger'],
      scripts=['queuelogger.py'],
      license="GNU General Public License 3",
      data_files=[
          ('/usr/share/pf-xivo-queues-logger/sqlite3/', ['db/create.sql'])
      ]
     )
