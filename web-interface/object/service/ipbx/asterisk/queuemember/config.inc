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

$array['element'] = array();

$array['element']['queue_name'] = array();
$array['element']['queue_name']['default'] = '';

$array['element']['interface'] = array();
$array['element']['interface']['default'] = '';

$array['element']['penalty'] = array();
$array['element']['penalty']['default'] = 0;
$array['element']['penalty']['value'] = range(0,20);
$array['element']['penalty']['setdef'] = true;

$array['element']['call-limit'] = array();
$array['element']['call-limit']['default'] = 0;
$array['element']['call-limit']['value'] = range(0,20);
$array['element']['call-limit']['setdef'] = true;

$array['element']['paused'] = array();
$array['element']['paused']['value'] = array(1,0);
$array['element']['paused']['default'] = '';
$array['element']['paused']['null'] = true;

$array['element']['chantype'] = array();
$array['element']['chantype']['default'] = 'default';
$array['element']['chantype']['value'] = array('default',XIVO_SRE_IPBX_AST_CHAN_LOCAL);
$array['element']['chantype']['unset'] = true;

$array['element']['usertype'] = array();
$array['element']['usertype']['value'] = array('user','agent');

$array['element']['userid'] = array();
$array['element']['userid']['default'] = 0;

$array['element']['channel'] = array();
$array['element']['channel']['value'] = array(
						XIVO_SRE_IPBX_AST_CHAN_SIP,
						XIVO_SRE_IPBX_AST_CHAN_IAX,
						XIVO_SRE_IPBX_AST_CHAN_LOCAL,
						XIVO_SRE_IPBX_AST_CHAN_AGENT,
						XIVO_SRE_IPBX_AST_CHAN_UNKNOWN);

$array['element']['category'] = array();
$array['element']['category']['value'] = array('queue','group');

$array['filter'] = array();
$array['filter']['queue_name'] = array('regexp' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['interface'] = array('regexp' => '/^[a-z0-9#\*\.\/_@:-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['penalty'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,20));
$array['filter']['call-limit'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,20));
$array['filter']['paused'] = array('set' => false,'chk' => 2,'bool' => true);
$array['filter']['usertype'] = array('key' => array('user','agent'));
$array['filter']['userid'] = array('cast' => 'ulongint');
$array['filter']['channel'] = array('key' => array(
							XIVO_SRE_IPBX_AST_CHAN_SIP,
							XIVO_SRE_IPBX_AST_CHAN_IAX,
							XIVO_SRE_IPBX_AST_CHAN_LOCAL,
							XIVO_SRE_IPBX_AST_CHAN_AGENT,
							XIVO_SRE_IPBX_AST_CHAN_UNKNOWN));
$array['filter']['category'] = array('key' => array('queue','group'));

?>
