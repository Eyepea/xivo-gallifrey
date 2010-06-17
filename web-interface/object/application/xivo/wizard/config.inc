<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

$array['step'] = array();
$array['step']['welcome'] = true;
$array['step']['license'] = true;
$array['step']['ipbxengine'] = true;
$array['step']['dbconfig'] = true;
$array['step']['checkcomponents'] = true;
$array['step']['mainconfig'] = true;
$array['step']['entitycontext'] = true;
$array['step']['ipbximportuser'] = true;
$array['step']['validate'] = true;

$array['ipbximportuser'] = array();
$array['ipbximportuser']['file'] = array();
$array['ipbximportuser']['file']['name'] = '/^[a-z0-9_-]{1,50}(?:\.[a-z]{1,10})?$/i';
$array['ipbximportuser']['file']['mime'] = array('text/plain','text/csv');

$array['ipbximportuser']['field'] = array();
$array['ipbximportuser']['field']['firstname'] = array('userfeatures','firstname');
$array['ipbximportuser']['field']['lastname'] = array('userfeatures','lastname');
$array['ipbximportuser']['field']['username'] = array('protocol','name');
$array['ipbximportuser']['field']['password'] = array('protocol','secret');
$array['ipbximportuser']['field']['phonenumber'] = array('userfeatures','number');
$array['ipbximportuser']['field']['incallexten'] = array('incall','exten');

$array['element'] = array();

$array['element']['dbconfig'] = array();
$array['element']['dbconfig']['backend']['value'] = array('mysql','sqlite');
$array['element']['dbconfig']['backend']['default'] = 'sqlite';

$array['element']['dbconfig']['sqlite'] = array();
$array['element']['dbconfig']['sqlite']['xivodb'] = array();
$array['element']['dbconfig']['sqlite']['xivodb']['default'] = '/var/lib/pf-xivo-web-interface/sqlite/xivo.db';

$array['element']['dbconfig']['sqlite']['ipbxdb'] = array();
$array['element']['dbconfig']['sqlite']['ipbxdb']['default'] = '/var/lib/asterisk/astsqlite';

$array['element']['dbconfig']['sqlite']['qldb'] = array();
$array['element']['dbconfig']['sqlite']['qldb']['default'] = '/var/lib/pf-xivo-queues-logger/sqlite3/queuestat.db';

$array['element']['dbconfig']['mysql'] = array();
$array['element']['dbconfig']['mysql']['host'] = array();
$array['element']['dbconfig']['mysql']['host']['default'] = 'localhost';

$array['element']['dbconfig']['mysql']['port'] = array();
$array['element']['dbconfig']['mysql']['port']['default'] = '';

$array['element']['dbconfig']['mysql']['xivouser'] = array();
$array['element']['dbconfig']['mysql']['xivouser']['default'] = '';

$array['element']['dbconfig']['mysql']['xivopass'] = array();
$array['element']['dbconfig']['mysql']['xivopass']['default'] = '';

$array['element']['dbconfig']['mysql']['xivodbname'] = array();
$array['element']['dbconfig']['mysql']['xivodbname']['default'] = 'xivo';

$array['element']['dbconfig']['mysql']['ipbxuser'] = array();
$array['element']['dbconfig']['mysql']['ipbxuser']['default'] = '';

$array['element']['dbconfig']['mysql']['ipbxpass'] = array();
$array['element']['dbconfig']['mysql']['ipbxpass']['default'] = '';

$array['element']['dbconfig']['mysql']['ipbxdbname'] = array();
$array['element']['dbconfig']['mysql']['ipbxdbname']['default'] = 'asterisk';

$array['element']['mainconfig'] = array();
$array['element']['mainconfig']['hostname'] = array();
$array['element']['mainconfig']['hostname']['default'] = '';

$array['element']['mainconfig']['domain'] = array();
$array['element']['mainconfig']['domain']['default'] = '';

$array['element']['mainconfig']['adminpasswd'] = array();
$array['element']['mainconfig']['adminpasswd']['default'] = '';

$array['element']['mainconfig']['confirmadminpasswd'] = array();
$array['element']['mainconfig']['confirmadminpasswd']['default'] = '';

$array['element']['netiface'] = array();
$array['element']['netiface']['address'] = array();
$array['element']['netiface']['address']['default'] = '';

$array['element']['netiface']['netmask'] = array();
$array['element']['netiface']['netmask']['default'] = '';

$array['element']['netiface']['gateway'] = array();
$array['element']['netiface']['gateway']['default'] = '';

$array['element']['resolvconf'] = array();
$array['element']['resolvconf']['nameserver1'] = array();
$array['element']['resolvconf']['nameserver1']['default'] = '';

$array['element']['resolvconf']['nameserver2'] = array();
$array['element']['resolvconf']['nameserver2']['default'] = '';

$array['element']['entity'] = array();
$array['element']['entity']['name'] = array();
$array['element']['entity']['name']['default'] = '';

$array['element']['entity']['displayname'] = array();
$array['element']['entity']['displayname']['default'] = '';

$array['element']['context'] = array();

$array['element']['context']['incall'] = array();
$array['element']['context']['incall']['name'] = array();
$array['element']['context']['incall']['name']['default'] = 'from-extern';

$array['element']['context']['incall']['displayname'] = array();
$array['element']['context']['incall']['displayname']['default'] = '';

$array['element']['context']['incall']['numberbeg'] = array();
$array['element']['context']['incall']['numberbeg']['default'] = '';

$array['element']['context']['incall']['numberend'] = array();
$array['element']['context']['incall']['numberend']['default'] = '';

$array['element']['context']['incall']['didlength'] = array();
$array['element']['context']['incall']['didlength']['value'] = range(1,20);
$array['element']['context']['incall']['didlength']['default'] = 4;

$array['element']['context']['internal'] = array();
$array['element']['context']['internal']['name'] = array();
$array['element']['context']['internal']['name']['default'] = 'default';

$array['element']['context']['internal']['displayname'] = array();
$array['element']['context']['internal']['displayname']['default'] = '';

$array['element']['context']['internal']['numberbeg'] = array();
$array['element']['context']['internal']['numberbeg']['default'] = '';

$array['element']['context']['internal']['numberend'] = array();
$array['element']['context']['internal']['numberend']['default'] = '';

$array['element']['context']['outcall'] = array();
$array['element']['context']['outcall']['name'] = array();
$array['element']['context']['outcall']['name']['default'] = 'to-extern';

$array['element']['context']['outcall']['displayname'] = array();
$array['element']['context']['outcall']['displayname']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();

$array['filter']['dbconfig'] = array();
$array['filter']['dbconfig']['backend'] = array('key' => array('sqlite','mysql'));

$array['filter']['dbconfig']['sqlite'] = array();
$array['filter']['dbconfig']['sqlite']['xivodb'] = array('minlen'	=> 4,
							 'maxlen'	=> 255,
							 'cast'		=> 'sgraph');
$array['filter']['dbconfig']['sqlite']['ipbxdb'] = array('minlen'	=> 4,
							 'maxlen'	=> 255,
							 'cast'		=> 'sgraph');
$array['filter']['dbconfig']['sqlite']['qldb'] = array('minlen'	=> 4,
							 'maxlen'	=> 255,
							 'cast'		=> 'sgraph');

$array['filter']['dbconfig']['mysql'] = array();
$array['filter']['dbconfig']['mysql']['host'] = array('callback'	=> 'dwho_network::valid_host',
						      'callbackarg'	=> array((DWHO_NETWORK_HOST_IPV4_DOTDEC |
										  DWHO_NETWORK_HOST_HOSTNAME)));
$array['filter']['dbconfig']['mysql']['port'] = array('set'		=> false,
						      'chk'		=> 2,
						      'callback'	=> 'dwho_network::valid_port_number');
$array['filter']['dbconfig']['mysql']['xivouser'] = array('set'		=> false,
							  'chk'		=> 2,
							  'maxlen'	=> 64,
							  'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/');
$array['filter']['dbconfig']['mysql']['xivopass'] = array('set'		=> false,
							  'chk'		=> 2,
							  'maxlen'	=> 64,
							  'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['filter']['dbconfig']['mysql']['xivodbname'] = array('minlen'	=> 1,
							    'maxlen'	=> 64,
							    'regexp'	=> '/^[a-zA-Z0-9_]+$/');
$array['filter']['dbconfig']['mysql']['ipbxuser'] = array('set'		=> false,
							  'chk'		=> 2,
							  'maxlen'	=> 64,
							  'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/');
$array['filter']['dbconfig']['mysql']['ipbxpass'] = array('set'		=> false,
							  'chk'		=> 2,
							  'maxlen'	=> 64,
							  'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['filter']['dbconfig']['mysql']['ipbxdbname'] = array('minlen'	=> 1,
							    'maxlen'	=> 64,
							    'regexp'	=> '/^[a-zA-Z0-9_]+$/');

$array['filter']['mainconfig'] = array();
$array['filter']['mainconfig']['hostname'] = array('callback'		=> 'dwho_network::valid_host',
						   'callbackarg'	=> array(DWHO_NETWORK_HOST_HOSTNAME));
$array['filter']['mainconfig']['domain'] = array('callback'		=> 'dwho_network::valid_host',
						 'callbackarg'		=> array(DWHO_NETWORK_HOST_HOSTNAME |
										 DWHO_NETWORK_HOST_HOSTNAME_TLD));
$array['filter']['mainconfig']['adminpasswd'] = array('minlen'	=> 4,
						      'maxlen'	=> 64,
						      'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/');
$array['filter']['mainconfig']['confirmadminpasswd'] = array('egal'	=> 'adminpasswd');

$array['filter']['netiface'] = array();
$array['filter']['netiface']['address'] = array('callback'	=> 'dwho_network::valid_ipv4_dotdec');
$array['filter']['netiface']['netmask'] = array('callback'	=> 'dwho_network::valid_mask_ipv4',
						'callbackarg'	=> array(DWHO_NETWORK_MASK_IPV4_DOTDEC));
$array['filter']['netiface']['gateway'] = array('set'		=> false,
						'chk'		=> 2,
						'callback'	=> 'dwho_network::valid_host',
						'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));

$array['filter']['resolvconf'] = array();
$array['filter']['resolvconf']['nameserver1'] = array('set'		=> 'false',
						      'chk'		=> 2,
						      'callback'	=> 'dwho_network::valid_host',
						      'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4 |
										 DWHO_NETWORK_HOST_HOSTNAME |
										 DWHO_NETWORK_HOST_HOSTNAME_TLD));
$array['filter']['resolvconf']['nameserver2'] = array('set'		=> 'false',
						      'chk'		=> 2,
						      'callback'	=> 'dwho_network::valid_host',
						      'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4 |
										 DWHO_NETWORK_HOST_HOSTNAME |
										 DWHO_NETWORK_HOST_HOSTNAME_TLD));
?>
