<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['iface'] = array();
$array['element']['iface']['default'] = '';

$array['element']['destination'] = array();
$array['element']['destination']['default'] = '';

$array['element']['netmask'] = array();
$array['element']['netmask']['default'] = '255.255.255.255';
$array['element']['netmask']['setdef'] = true;

$array['element']['gateway'] = array();
$array['element']['gateway']['default'] = '';

$array['element']['disable'] = array();
$array['element']['disable']['value'] = array(0,1);
$array['element']['disable']['default'] = 0;
$array['element']['disable']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/','minlen' => 1,'maxlen' => 64);
$array['filter']['iface'] = array('regexp' => '/^[a-z0-9_]+(?:\:[0-9]+)?$/','minlen' => 1,'maxlen' => 64);
$array['filter']['destination'] = array('callback'	=> 'dwho_network::valid_host',
					'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['netmask'] = array('set'		=> false,
				    'chk'		=> 2,
				    'callback'		=> 'dwho_network::valid_mask_ipv4',
				    'callbackarg'	=> array(DWHO_NETWORK_MASK_IPV4_DOTDEC));
$array['filter']['gateway'] = array('callback'		=> 'dwho_network::valid_host',
				    'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['disable'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
