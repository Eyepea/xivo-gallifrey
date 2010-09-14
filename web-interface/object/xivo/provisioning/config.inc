<?php

#
# XiVO Web-Interface
# Copyright (C) 2020  Proformatique <technique@proformatique.com>
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

$array['element']['registrar_main'] = array();
$array['element']['registrar_main']['default'] = '';

$array['element']['registrar_backup'] = array();
$array['element']['registrar_backup']['default'] = '';

$array['element']['proxy_main'] = array();
$array['element']['proxy_main']['default'] = '';

$array['element']['proxy_backup'] = array();
$array['element']['proxy_backup']['default'] = '';

$array['element']['vlan_id'] = array();
$array['element']['vlan_id']['default'] = '';
$array['element']['vlan_id']['null']    = true;

### filters ###
$array['filter'] = array();

$array['filter']['registrar_main']	 = array('set'		=> false,
				    'chk'		    => 2,
				    'callback'		=> 'dwho_network::valid_host',
				    'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['registrar_backup'] = array('set'		=> false,
				    'chk'		    => 2,
				    'callback'		=> 'dwho_network::valid_host',
				    'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['proxy_main']	     = array('set'		=> false,
				    'chk'		    => 2,
				    'callback'		=> 'dwho_network::valid_host',
				    'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['proxy_backup']	   = array('set'		=> false,
				    'chk'		    => 2,
				    'callback'		=> 'dwho_network::valid_host',
						'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['vlan_id']          = array('set'    => false,
						'chk'			=> 2,
						'cast'    => 'uint',
						'between' => array(1, 4094));
?>
