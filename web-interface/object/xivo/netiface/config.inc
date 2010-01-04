<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

$array['element']['devname'] = array();
$array['element']['devname']['default'] = '';

$array['element']['hwtypeid'] = array();
$array['element']['hwtypeid']['default'] = 65534;

$array['element']['networktype'] = array();
$array['element']['networktype']['value'] = array('data','voip');
$array['element']['networktype']['default'] = 'data';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('iface');
$array['element']['type']['default'] = 'iface';
$array['element']['type']['forcedef'] = true;

$array['element']['family'] = array();
$array['element']['family']['value'] = array('inet','inet6');
$array['element']['family']['default'] = 'inet';
$array['element']['family']['forcedef'] = true;

$array['element']['method'] = array();
$array['element']['method']['value'] = array('static','dhcp');
$array['element']['method']['default'] = 'static';

$array['element']['address'] = array();
$array['element']['address']['default'] = '';
$array['element']['address']['null'] = true;

$array['element']['netmask'] = array();
$array['element']['netmask']['default'] = '';
$array['element']['netmask']['null'] = true;

$array['element']['broadcast'] = array();
$array['element']['broadcast']['default'] = '';
$array['element']['broadcast']['null'] = true;

$array['element']['gateway'] = array();
$array['element']['gateway']['default'] = '';
$array['element']['gateway']['null'] = true;

$array['element']['mtu'] = array();
$array['element']['mtu']['default'] = '';
$array['element']['mtu']['null'] = true;

$array['element']['vlanrawdevice'] = array();
$array['element']['vlanrawdevice']['default'] = '';
$array['element']['vlanrawdevice']['null'] = true;

$array['element']['vlanid'] = array();
$array['element']['vlanid']['default'] = '';
$array['element']['vlanid']['null'] = true;

$array['element']['advconfig'] = array();
$array['element']['advconfig']['default'] = '';

$array['element']['disable'] = array();
$array['element']['disable']['value'] = array(0,1);
$array['element']['disable']['default'] = 0;
$array['element']['disable']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/','minlen' => 1,'maxlen' => 64);
$array['filter']['devname'] = array('regexp' => '/^[a-z0-9_]+(?:\:[0-9]+)?$/','minlen' => 1,'maxlen' => 64);
$array['filter']['hwtypeid'] = array('cast' => 'uint','between' => array(0,65535));
$array['filter']['networktype'] = array('key' => array('data','voip'));
$array['filter']['type'] = array('set' => false,'key' => array('iface'));
$array['filter']['family'] = array('set' => false,'key' => array('inet','inet6'));
$array['filter']['method'] = array('key' => array('static','dhcp'));
$array['filter']['address'] = array('set'		=> false,
				    'chk'		=> 2,
				    'callback'		=> 'dwho_network::valid_host',
				    'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['netmask'] = array('set'		=> false,
				    'chk'		=> 2,
				    'callback'		=> 'dwho_network::valid_mask_ipv4',
				    'callbackarg'	=> array(DWHO_NETWORK_MASK_IPV4_DOTDEC));
$array['filter']['broadcast'] = array('set'		=> false,
				      'chk'		=> 2,
				      'callback'	=> 'dwho_network::valid_host',
				      'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['gateway'] = array('set'		=> false,
				    'chk'		=> 2,
				    'callback'		=> 'dwho_network::valid_host',
				    'callbackarg'	=> array(DWHO_NETWORK_HOST_IPV4_DOTDEC));
$array['filter']['mtu'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(68,9000));
$array['filter']['vlanrawdevice'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_]+$/','minlen' => 1,'maxlen' => 64);
$array['filter']['vlanid'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,4094));
$array['filter']['advconfig'] = array('set' => false,'chk' => 2,'maxlen' => 2000,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['disable'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
