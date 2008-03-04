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
      # distutils unable to send two scripts at two different places :/
      # scripts=['autoprov', 'initconfig'],
      data_files=[('/usr/share/pf-xivo-provisioning/files',
			['files/linksys-pap2t.cfg',
			 'files/linksys-spa901.cfg',
			 'files/linksys-spa921.cfg',
			 'files/linksys-spa922.cfg',
			 'files/linksys-spa941.cfg',
			 'files/linksys-spa942.cfg',
			 'files/linksys-spa962.cfg',
			 'files/polycom-phone.cfg',
			 'files/polycom-spip_430.cfg',
			 'files/polycom-spip_650.cfg',
			 'files/snom-template.htm',
			 'files/ST2022S_template.txt',
			 'files/ST2030S_template.txt',
			 'files/template_ip10.cfg',
			 'files/template_ip10.inf',
			]),
		  ('/etc/pf-xivo', ['etc/pf-xivo/provisioning.conf']),
		  ('/etc/apache2', ['etc/apache2/autoprov.conf']),
		  ('/etc/apache2', ['etc/apache2/ports.conf']),
		  ('/etc/dhcp3', ['etc/dhcp3/dhcpd.conf']),
		  ('/usr/sbin', ['autoprov']),
		  ('/usr/share/asterisk/agi-bin/', ['initconfig']),
		 ],
     )
