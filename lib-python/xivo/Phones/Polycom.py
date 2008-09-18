"""Support for Polycom phones for XIVO Configuration

Polycom SoundPoint IP 430 SIP and SoundPoint IP 650 SIP are supported.

Copyright (C) 2007, 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

import os
import socket

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_config import ProvGeneralConf as Pgc

POLYCOM_COMMON_DIR = os.path.join(Pgc['tftproot'], "Polycom/")
POLYCOM_COMMON_HTTP_USER = "Polycom"
POLYCOM_COMMON_HTTP_PASS = "456"
SIP_PORT = 5060
AMI_PORT = 5038
AMI_USER = 'xivouser'
AMI_PASS = 'xivouser'

class Polycom(PhoneVendorMixin):

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        # TODO: handle this with a lookup table stored in the DB?
        if self.phone['model'] != 'spip_430' and \
           self.phone['model'] != 'spip_650':
            raise ValueError, "Unknown Polycom model %r" % self.phone['model']

    def __retrieve_peerinfo(self):
        peerinfo = None
        phoneip = self.phone['ipv4']

        # TODO: get ride of this AMI communication if possible

        amisock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        amisock.settimeout(float(Pgc['telnet_to_s']))
        amisock.connect(("127.0.0.1", AMI_PORT))
        actioncommand = ( "Action: login\r\n"
                          "Username: %s\r\n"
                          "Secret: %s\r\n"
                          "Events: off\r\n"
                          "\r\n"
                          "Action: SIPpeers\r\n"
                          "\r\n" ) \
                        % (AMI_USER, AMI_PASS)
        amisock.send(actioncommand)

        fullmsg = ''
        iquit = False
        ipaddressmatch = 'IPaddress: %s' % phoneip

        # finds the IP address matching phoneip among the peers
        while True:
            msg = amisock.recv(8192)
            if len(msg) > 0:
                fullmsg += msg
                events = fullmsg.split("\r\n\r\n")
                fullmsg = events.pop()
                for ev in events:
                    lines = ev.split("\r\n")
                    if ipaddressmatch in lines:
                        for myline in lines:
                            if myline.startswith("ObjectName: "):
                                peerinfo = myline.split(": ")
                                iquit = True
                    if "Event: PeerlistComplete" in lines:
                        iquit = True
                    if iquit:
                        break
                if iquit:
                    break
            else:
                break
        amisock.close()
        return peerinfo

    def __sendsipnotify(self):
        phoneip = self.phone['ipv4']
        myip = Pgc['asterisk_ipv4']

        peerinfo = None
        try:
            peerinfo = self.__retrieve_peerinfo()
        except Exception: # XXX: don't catch ALL exceptions
            pass

        if peerinfo is not None and len(peerinfo) > 1:
            sip_number = peerinfo[1]
        else:
            sip_number = 'guest'

        sip_message = ( "NOTIFY sip:%s@%s:%d SIP/2.0\r\n"
                        "Via: SIP/2.0/UDP %s\r\n"
                        "From: <sip:%s@%s>\r\n"
                        "To: <sip:%s@%s>\r\n"
                        "Event: check-sync\r\n"
                        "Call-ID: callid-reboot@%s\r\n"
                        "CSeq: 1300 NOTIFY\r\n"
                        "Contact: <sip:%s@%s>\r\n"
                        "Content-Length: 0\r\n" ) \
                      % (sip_number, phoneip, SIP_PORT,
                         myip,
                         sip_number, myip,
                         sip_number, phoneip,
                         myip,
                         sip_number, myip)
        sipsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sipsocket.sendto(sip_message, (phoneip, SIP_PORT))

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__sendsipnotify()

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__sendsipnotify()

    def __generate(self, provinfo):
        template_main_file = open(os.path.join(Pgc['templates_dir'], "polycom-%s.cfg" % self.phone['model']))
        template_main_lines = template_main_file.readlines()
        template_main_file.close()
        template_phone_file = open(os.path.join(Pgc['templates_dir'], "polycom-phone.cfg"))
        template_phone_lines = template_phone_file.readlines()
        template_phone_file.close()

        macaddr = self.phone['macaddr'].replace(":", "").lower()
        tmp_main_filename = os.path.join(POLYCOM_COMMON_DIR, macaddr + ".cfg.tmp")
        cfg_main_filename = tmp_main_filename[:-4]
        tmp_phone_filename = os.path.join(POLYCOM_COMMON_DIR, macaddr + "-phone.cfg.tmp")
        cfg_phone_filename = tmp_phone_filename[:-4]

        txt_main = xivo_config.txtsubst(template_main_lines,
                { 'phone.cfg': macaddr + "-phone.cfg" },
                cfg_main_filename)
        txt_phone = xivo_config.txtsubst(template_phone_lines,
                { 'user_display_name': provinfo['name'],
                  'user_phone_ident':  provinfo['ident'],
                  'user_phone_number': provinfo['number'],
                  'user_phone_passwd': provinfo['passwd'],
                  'asterisk_ipv4' : Pgc['asterisk_ipv4']
                },
                cfg_phone_filename)

        tmp_main_file = open(tmp_main_filename, "w")
        tmp_main_file.writelines(txt_main)
        tmp_main_file.close()
        tmp_phone_file = open(tmp_phone_filename, "w")
        tmp_phone_file.writelines(txt_phone)
        tmp_phone_file.close()

        os.rename(tmp_main_filename, cfg_main_filename)
        os.rename(tmp_phone_filename, cfg_phone_filename)

    def do_reinitprov(self):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        self.__generate(
                { 'name':   "guest",
                  'ident':  "guest",
                  'number': "guest",
                  'passwd': "guest",
                })

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        self.__generate(provinfo)

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return (('spip_430', 'SPIP430'), ('spip_650', 'SPIP650'))

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # PolycomSoundPointIP-SPIP_430-UA/2.2.0.0047
        # PolycomSoundPointIP-SPIP_650-UA/2.2.0.0047

        if ua[:7] != 'Polycom':
            return None
        model = 'unknown'
        fw = 'unknown'
        ua_splitted = ua.split('/', 1)
        if len(ua_splitted) == 2:
            fw = ua_splitted[1]
            model = ua_splitted[0].split('-')[1].lower()
        return ("polycom", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        yield 'subclass "phone-mac-address-prefix" 1:00:04:f2 {\n'
        yield '    log("class Polycom prefix 1:00:04:f2");\n'
        yield '    option tftp-server-name "tftp://%s/Polycom";\n' % addresses['bootServer']
        yield '}\n'
        yield '\n'

    @classmethod
    def get_dhcp_pool_lines(cls):
        return ()

xivo_config.register_phone_vendor_class(Polycom)
