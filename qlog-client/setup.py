#!/usr/bin/env python
# -*- coding: UTF-8 -*-


from distutils.core import setup

setup(name='qlog_client',
      version='1.0',
      description='XIVO utility to send queuelog and agent data to a remote server',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      packages=['qlogclient'],
      package_dir={'': 'src'},
      scripts=['scripts/qlogc'],
      data_files=[
                  ('/etc/pf-xivo', ['etc/pf-xivo/qlog-client.conf']),
                  ('/etc/cron.d', ['etc/cron.d/pf-xivo-qlog-client']),
                 ],
     )
