#!/usr/bin/env python

__version__ = "$Revision$ $Date$"

from distutils.core import setup

setup(name='xivo_provisioning',
      version='1.0',
      description='XIVO Provisioning daemon, AGI and modules',
      author='Proformatique',
      author_email='technique@proformatique.com',
      url='http://xivo.fr/',
      
      ## distutils unable to send two scripts at two different places :/
      # scripts=['autoprov', 'initconfig'],
      
      data_files=[
            ('/usr/share/pf-xivo-provisioning/files',
                ['files/aastra-6730i.cfg',
                 'files/aastra-6731i.cfg',
                 'files/aastra-6739i.cfg',
                 'files/aastra-6751i.cfg',
                 'files/aastra-6753i.cfg',
                 'files/aastra-6755i.cfg',
                 'files/aastra-6757i.cfg',
                 'files/linksys-pap2t.cfg',
                 'files/linksys-spa901.cfg',
                 'files/linksys-spa921.cfg',
                 'files/linksys-spa922.cfg',
                 'files/linksys-spa941.cfg',
                 'files/linksys-spa942.cfg',
                 'files/linksys-spa962.cfg',
                 'files/linksys-spa3102.cfg',
                 'files/polycom-phone.cfg',
                 'files/siemens-c470ip.ini',
                 'files/siemens-s675ip.ini',
                 'files/snom-template.htm',
                 'files/ST2022S_template.txt',
                 'files/ST2030S_template.txt',
                 'files/TB30S_template.txt',
                 'files/template_ip10.cfg',
                 'files/template_ip10.inf',
                 'files/yealink-t20p.cfg',
                 'files/yealink-t22p.cfg',
                 'files/yealink-t26p.cfg',
                 'files/yealink-t28p.cfg',
                 'files/cisco-cp7912g.cfg',
                 'files/cisco-cp7940g.cfg',
                 'files/cisco-cp7960g.cfg',
                 'files/cisco-cp7941g.cfg',
                 'files/cisco-cp7961g.cfg',
                 'files/sccp-cisco-cp7912g.cfg',
                 'files/sccp-cisco-cp7941g.cfg',
                 'files/sccp-cisco-cp7960g.cfg',
                 'files/sccp-cisco-cp7961g.cfg',
                 'files/sccp-cisco-cipc.cfg',
                 'files/sccp-cisco-addons.cfg',
            ]),
            ('/etc/pf-xivo', ['etc/pf-xivo/provisioning.conf']),
            ('/usr/sbin', ['autoprov']),
            ('/usr/share/asterisk/agi-bin', ['bin/initconfig']),
            ('/usr/share/pf-xivo-provisioning/bin', ['bin/dhcpconfig'])
         ],
     )
