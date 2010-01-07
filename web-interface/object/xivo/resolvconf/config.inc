<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

$array['element'] = array();

$array['element']['hostname'] = array();
$array['element']['hostname']['default'] = php_uname('n');

$array['element']['nameserver1'] = array();
$array['element']['nameserver1']['default'] = '';
$array['element']['nameserver1']['null'] = true;

$array['element']['nameserver2'] = array();
$array['element']['nameserver2']['default'] = '';
$array['element']['nameserver2']['null'] = true;

$array['element']['nameserver3'] = array();
$array['element']['nameserver3']['default'] = '';
$array['element']['nameserver3']['null'] = true;

$array['element']['search'] = array();
$array['element']['search']['null'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['hostname'] = array('callback' => 'dwho_network::valid_hostname_part');
$array['filter']['nameserver1'] = array('set'		=> 'false',
					'chk'		=> 2,
					'callback'	=> 'dwho_network::valid_host',
					'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4 |
								 DWHO_NETWORK_HOST_HOSTNAME |
								 DWHO_NETWORK_HOST_HOSTNAME_TLD));
$array['filter']['nameserver2'] = array('set'		=> 'false',
					'chk'		=> 2,
					'callback'	=> 'dwho_network::valid_host',
					'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4 |
								 DWHO_NETWORK_HOST_HOSTNAME |
								 DWHO_NETWORK_HOST_HOSTNAME_TLD));
$array['filter']['nameserver3'] = array('set'		=> 'false',
					'chk'		=> 2,
					'callback'	=> 'dwho_network::valid_host',
					'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4 |
								 DWHO_NETWORK_HOST_HOSTNAME |
								 DWHO_NETWORK_HOST_HOSTNAME_TLD));
$array['filter']['search'] = array('set'		=> false,
				   'chk'		=> 2,
				   'maxlen'		=> 255,
				   'callback'		=> 'dwho_network::valid_hostname_list',
				   'callbackarg'	=> array(DWHO_NETWORK_HOST_HOSTNAME |
				   				 DWHO_NETWORK_HOST_HOSTNAME_TLD));
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
