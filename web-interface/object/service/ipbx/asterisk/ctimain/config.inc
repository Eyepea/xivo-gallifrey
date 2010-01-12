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
$array['element'] = array();

$array['element']['host'] = array();
$array['element']['host']['default'] = '';

$array['element']['commandset'] = array();
$array['element']['commandset']['value'] = array('xivocti','callbooster');
$array['element']['commandset']['default'] = 'xivocti';

$array['element']['fagi_ip'] = array();
$array['element']['fagi_ip']['default'] = '127.0.0.1';
$array['element']['fagi_port'] = array();
$array['element']['fagi_port']['default'] = 5002;

$array['element']['cti_ip'] = array();
$array['element']['cti_ip']['default'] = '0.0.0.0';
$array['element']['cti_port'] = array();
$array['element']['cti_port']['default'] = 5003;

$array['element']['webi_ip'] = array();
$array['element']['webi_ip']['default'] = '127.0.0.1';
$array['element']['webi_port'] = array();
$array['element']['webi_port']['default'] = 5004;

$array['element']['info_ip'] = array();
$array['element']['info_ip']['default'] = '127.0.0.1';
$array['element']['info_port'] = array();
$array['element']['info_port']['default'] = 5005;

$array['element']['announce_ip'] = array();
$array['element']['announce_ip']['default'] = '127.0.0.1';
$array['element']['announce_port'] = array();
$array['element']['announce_port']['default'] = 5006;

$array['element']['asterisklist'] = array();
$array['element']['asterisklist']['default'] = 'xivo';
$array['element']['asterisklist']['value'] = array('xivo');

$array['element']['updates_period'] = array();
$array['element']['updates_period']['default'] = 60;

$array['element']['socket_timeout'] = array();
$array['element']['socket_timeout']['default'] = 10;

$array['element']['login_timeout'] = array();
$array['element']['login_timeout']['default'] = 5;

dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['commandset'] 		= array('key' => array('xivocti','callbooster'));
$array['filter']['fagi_ip'] 		= array('callback' => 'dwho_network::valid_host_subnet_ipv4');
$array['filter']['fagi_port'] 		= array('set' => false, 'chk' => 2, 'cast' => 'uint', 'between' => array(1,65535));
$array['filter']['cti_ip'] 			= array('callback' => 'dwho_network::valid_host_subnet_ipv4');
$array['filter']['cti_port'] 		= array('set' => false, 'chk' => 2, 'cast' => 'uint', 'between' => array(1,65535));
$array['filter']['webi_ip'] 		= array('callback' => 'dwho_network::valid_host_subnet_ipv4');
$array['filter']['webi_port'] 		= array('set' => false, 'chk' => 2, 'cast' => 'uint', 'between' => array(1,65535));
$array['filter']['info_ip'] 		= array('callback' => 'dwho_network::valid_host_subnet_ipv4');
$array['filter']['info_port'] 		= array('set' => false, 'chk' => 2, 'cast' => 'uint', 'between' => array(1,65535));
$array['filter']['announce_ip'] 	= array('callback' => 'dwho_network::valid_host_subnet_ipv4');
$array['filter']['announce_port'] 	= array('set' => false, 'chk' => 2, 'cast' => 'uint', 'between' => array(1,65535));
#$array['filter']['asterisklist'] 	= array('set' => false, 'chk' => 2, 'key' => array('xivo'));
$array['filter']['asterisklist'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['updates_period'] 	= array('set' => false, 'chk' => 2, 'cast' => 'uint', 'between' => array(1,120));
$array['filter']['socket_timeout'] 	= array('set' => false, 'chk' => 2, 'cast' => 'uint', 'between' => array(1,20));
$array['filter']['login_timeout'] 	= array('set' => false, 'chk' => 2, 'cast' => 'uint', 'between' => array(1,20));
$array['filter']['parting_astid_context'] 
									= array('set' => false, 'chk' => 2, 'key' => array('astid', 'context', 'context,astid'));
?>
