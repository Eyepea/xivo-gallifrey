<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$array = array();

$array['provisioning_id'] = array();

$array['provisioning_id']['sip'] = array();
$array['provisioning_id']['sip']['prefix'] = 1;
$array['provisioning_id']['sip']['range'] = 100000;

$array['provisioning_id']['iax'] = array();
$array['provisioning_id']['iax']['prefix'] = 2;
$array['provisioning_id']['iax']['range'] = 100000;

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['protocol'] = array();
$array['element']['protocol']['value'] = array(XIVO_SRE_IPBX_AST_PROTO_SIP,
					       XIVO_SRE_IPBX_AST_PROTO_IAX,
					       XIVO_SRE_IPBX_AST_PROTO_CUSTOM);
$array['element']['protocol']['default'] = XIVO_SRE_IPBX_AST_PROTO_SIP;

$array['element']['protocolid'] = array();
$array['element']['protocolid']['default'] = 0;

$array['element']['provisioningid'] = array();
$array['element']['provisioningid']['default'] = 0;
$array['element']['provisioningid']['null'] = true;

$array['element']['firstname'] = array();
$array['element']['firstname']['default'] = '';

$array['element']['lastname'] = array();
$array['element']['lastname']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['voicemailid'] = array();
$array['element']['voicemailid']['default'] = 0;
$array['element']['voicemailid']['null'] = true;

$array['element']['agentid'] = array();
$array['element']['agentid']['default'] = 0;
$array['element']['agentid']['null'] = true;

$array['element']['ringseconds'] = array();
$array['element']['ringseconds']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['ringseconds']['default'] = 30;
$array['element']['ringseconds']['setdef'] = true;

$array['element']['simultcalls'] = array();
$array['element']['simultcalls']['value'] = array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
$array['element']['simultcalls']['default'] = 5;
$array['element']['simultcalls']['setdef'] = true;

$array['element']['enableclient'] = array();
$array['element']['enableclient']['value'] = array(0,1);
$array['element']['enableclient']['default'] = 0;
$array['element']['enableclient']['set'] = true;

$array['element']['loginclient'] = array();
$array['element']['loginclient']['default'] = '';

$array['element']['passwdclient'] = array();
$array['element']['passwdclient']['default'] = '';

$array['element']['profileclient'] = array();
$array['element']['profileclient']['default'] = 'client';
$array['element']['profileclient']['setdef'] = true;

$array['element']['enablehint'] = array();
$array['element']['enablehint']['value'] = array(0,1);
$array['element']['enablehint']['default'] = 1;
$array['element']['enablehint']['set'] = true;

$array['element']['enablevoicemail'] = array();
$array['element']['enablevoicemail']['value'] = array(0,1);
$array['element']['enablevoicemail']['default'] = 0;
$array['element']['enablevoicemail']['set'] = true;

$array['element']['enablexfer'] = array();
$array['element']['enablexfer']['value'] = array(0,1);
$array['element']['enablexfer']['default'] = 0;
$array['element']['enablexfer']['set'] = true;

$array['element']['enableautomon'] = array();
$array['element']['enableautomon']['value'] = array(0,1);
$array['element']['enableautomon']['default'] = 0;
$array['element']['enableautomon']['set'] = true;

$array['element']['callrecord'] = array();
$array['element']['callrecord']['value'] = array(0,1);
$array['element']['callrecord']['default'] = 0;
$array['element']['callrecord']['set'] = true;

$array['element']['incallfilter'] = array();
$array['element']['incallfilter']['value'] = array(0,1);
$array['element']['incallfilter']['default'] = 0;
$array['element']['incallfilter']['set'] = true;

$array['element']['enablednd'] = array();
$array['element']['enablednd']['value'] = array(0,1);
$array['element']['enablednd']['default'] = 0;
$array['element']['enablednd']['set'] = true;

$array['element']['enablerna'] = array();
$array['element']['enablerna']['value'] = array(0,1);
$array['element']['enablerna']['default'] = 0;
$array['element']['enablerna']['set'] = true;

$array['element']['destrna'] = array();
$array['element']['destrna']['default'] = '';

$array['element']['enablebusy'] = array();
$array['element']['enablebusy']['value'] = array(0,1);
$array['element']['enablebusy']['default'] = 0;
$array['element']['enablebusy']['set'] = true;

$array['element']['destbusy'] = array();
$array['element']['destbusy']['default'] = '';

$array['element']['enableunc'] = array();
$array['element']['enableunc']['value'] = array(0,1);
$array['element']['enableunc']['default'] = 0;
$array['element']['enableunc']['set'] = true;

$array['element']['destunc'] = array();
$array['element']['destunc']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = XIVO_SRE_IPBX_AST_MOH_DEFAULT;

$array['element']['outcallerid'] = array();
$array['element']['outcallerid']['value'] = array('default','anonymous');
$array['element']['outcallerid']['default'] = 'default';
$array['element']['outcallerid']['setdef'] = true;

$array['element']['outcallerid-type'] = array();
$array['element']['outcallerid-type']['value'] = array('default','anonymous','custom');
$array['element']['outcallerid-type']['default'] = 'default';
$array['element']['outcallerid-type']['unset'] = true;

$array['element']['outcallerid-custom'] = array();
$array['element']['outcallerid-custom']['default'] = '';
$array['element']['outcallerid-custom']['unset'] = true;

$array['element']['mobilephonenumber'] = array();
$array['element']['mobilephonenumber']['default'] = '';

$array['element']['bsfilter'] = array();
$array['element']['bsfilter']['value'] = array('no','boss','secretary');
$array['element']['bsfilter']['default'] = 'no';
$array['element']['bsfilter']['setdef'] = true;

$array['element']['preprocess_subroutine'] = array();
$array['element']['preprocess_subroutine']['default'] = '';
$array['element']['preprocess_subroutine']['null'] = true;

$array['element']['internal'] = array();
$array['element']['internal']['value'] = array(0,1);
$array['element']['internal']['default'] = 0;
$array['element']['internal']['forcedef'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9#\*\.\/_@:-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['protocol'] = array('key' => array(XIVO_SRE_IPBX_AST_PROTO_SIP,
						    XIVO_SRE_IPBX_AST_PROTO_IAX,
						    XIVO_SRE_IPBX_AST_PROTO_CUSTOM));
$array['filter']['protocolid'] = array('cast' => 'ulongint');
$array['filter']['firstname'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['lastname'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['number'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(
				    			XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['voicemailid'] = array('set' => false,'chk' => 2,'cast' => 'ulongint');
$array['filter']['agentid'] = array('set' => false,'chk' => 2,'cast' => 'ulongint');
$array['filter']['ringseconds'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['simultcalls'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,20));
$array['filter']['enableclient'] = array('bool' => true);
$array['filter']['loginclient'] = array('set'		=> false,
					'chk'		=> 2,
					'minlen'	=> 4,
					'maxlen'	=> 64,
					'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/');
$array['filter']['passwdclient'] = array('set'		=> false,
					 'chk'		=> 2,
					 'minlen'	=> 4,
					 'maxlen'	=> 64,
					 'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['filter']['profileclient'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-]+$/','maxlen' => 64);
$array['filter']['enablehint'] = array('bool' => true);
$array['filter']['enablevoicemail'] = array('bool' => true);
$array['filter']['enablexfer'] = array('bool' => true);
$array['filter']['enableautomon'] = array('bool' => true);
$array['filter']['callrecord'] = array('bool' => true);
$array['filter']['incallfilter'] = array('bool' => true);
$array['filter']['enablednd'] = array('bool' => true);
$array['filter']['enablerna'] = array('bool' => true);
$array['filter']['destrna'] = array('set' => false,'chk' => 2,'regexp' => '/^\+?[0-9\*#]+$/','maxlen' => 128);
$array['filter']['enablebusy'] = array('bool' => true);
$array['filter']['destbusy'] = array('set' => false,'chk' => 2,'regexp' => '/^\+?[0-9\*#]+$/','maxlen' => 128);
$array['filter']['enableunc'] = array('bool' => true);
$array['filter']['destunc'] = array('set' => false,'chk' => 2,'regexp' => '/^\+?[0-9\*#]+$/','maxlen' => 128);
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 20);
$array['filter']['outcallerid'] = array('set'		=> false,
					'chk'		=> 2,
					'maxlen'	=> 80,
					'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_CALLERID.'$/u',
					'notag'		=> false);
$array['filter']['bsfilter'] = array('set' => false,'chk' => 2,'key' => array('no','boss','secretary'));
$array['filter']['mobilephonenumber'] = array('set'	=> false,
					      'chk'	=> 2,
					      'regexp'	=> '/^\+?[0-9\*#]+$/',
					      'maxlen'	=> 128);
$array['filter']['preprocess_subroutine'] = array('set' => false,
						  'chk'	=> 2,
						  'callback' => 'xivo_service_asterisk::chk_context',
						  'callbackarg' => array(array(
							XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['internal'] = array('set' => false,'chk' => 0);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
