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

$array['element']['name'] = array();
$array['element']['name']['default'] = '*none*';

$array['element']['devicetype'] = array();
$array['element']['devicetype']['default'] = '';
$array['element']['devicetype']['value'] = array('12', '30', '7902', '7905', '7906', '7910', '7911', '7912', '7935', '7936', '7937', '7920', '7921', '7931', '7940', '7941', '7942', '7945', '7960', '7961', '7962', '7965', '7970', '7971', '7975', '7985', 'ata', 'kirk', 'cipc', 'nokia-icc');

$array['element']['keepalive'] = array();
$array['element']['keepalive']['default'] = '';
$array['element']['keepalive']['null'] = true;

$array['element']['tzoffset'] = array();
$array['element']['tzoffset']['default'] = '';
$array['element']['tzoffset']['value'] = array('-12', '-11', '-10', '-9', '-8', '-7', '-6', '-5', '-4', '-3', '-2', '-1', '0', '+1', '+2', '+3', '+4', '+5', '+6', '+7', '+8', '+9', '+10', '+11', '+12');

$array['element']['dtmfmode'] = array();
$array['element']['dtmfmode']['default'] = '';
$array['element']['dtmfmode']['value'] = array('outofband', 'inband');

$array['element']['transfer'] = array();
$array['element']['transfer']['default'] = '';
$array['element']['transfer']['value'] = array('on', 'off');

$array['element']['park'] = array();
$array['element']['park']['default'] = '';
$array['element']['park']['value'] = array('on', 'off');

$array['element']['cfwdall'] = array();
$array['element']['cfwdall']['default'] = '';
$array['element']['cfwdall']['value'] = array('on', 'off');

$array['element']['cfwdbusy'] = array();
$array['element']['cfwdbusy']['default'] = '';
$array['element']['cfwdbusy']['value'] = array('on', 'off');

$array['element']['cfwdnoanswer'] = array();
$array['element']['cfwdnoanswer']['default'] = '';
$array['element']['cfwdnoanswer']['value'] = array('on', 'off');

$array['element']['mwilamp'] = array();
$array['element']['mwilamp']['default'] = '';
$array['element']['mwilamp']['value'] = array('on', 'off', 'wink', 'flash', 'blink');

$array['element']['mwioncall'] = array();
$array['element']['mwioncall']['default'] = '';
$array['element']['mwioncall']['value'] = array('on', 'off');

$array['element']['dnd'] = array();
$array['element']['dnd']['default'] = '';
$array['element']['dnd']['value'] = array('on', 'off', 'reject', 'silent');

$array['element']['pickupexten'] = array();
$array['element']['pickupexten']['default'] = '';
$array['element']['pickupexten']['value'] = array('on', 'off');

$array['element']['pickupcontext'] = array();
$array['element']['pickupcontext']['default'] = '';

$array['element']['pickupmodeanswer'] = array();
$array['element']['pickupmodeanswer']['default'] = '';
$array['element']['pickupmodeanswer']['value'] = array('on', 'off');

$array['element']['permit'] = array();
$array['element']['permit']['default'] = '';
$array['element']['permit']['null'] = true;

$array['element']['deny'] = array();
$array['element']['deny']['default'] = '';
$array['element']['deny']['null'] = true;

$array['element']['addons'] = array();
$array['element']['addons']['default'] = '';

$array['element']['imageversion'] = array();
$array['element']['imageversion']['default'] = '';

$array['element']['trustphoneip'] = array();
$array['element']['trustphoneip']['default'] = '';
$array['element']['trustphoneip']['value'] = array('yes', 'no');

$array['element']['nat'] = array();
$array['element']['nat']['default'] = '';
$array['element']['nat']['value'] = array('on', 'off');

$array['element']['directrtp'] = array();
$array['element']['directrtp']['default'] = '';
$array['element']['directrtp']['value'] = array('on', 'off');

$array['element']['earlyrtp'] = array();
$array['element']['earlyrtp']['default'] = '';
$array['element']['earlyrtp']['value'] = array('none', 'offhook', 'dial', 'ringout');

$array['element']['private'] = array();
$array['element']['private']['default'] = '';
$array['element']['private']['value'] = array('on', 'off');

$array['element']['privacy'] = array();
$array['element']['privacy']['default'] = '';
$array['element']['privacy']['value'] = array('on', 'off', 'full');

$array['element']['protocol'] = array();
$array['element']['protocol']['default'] = XIVO_SRE_IPBX_AST_PROTO_SCCP;
$array['element']['protocol']['forcedef'] = true;

$array['element']['softkey_onhook'] = array();
$array['element']['softkey_onhook']['default'] = '';

$array['element']['softkey_connected'] = array();
$array['element']['softkey_connected']['default'] = '';

$array['element']['softkey_onhold'] = array();
$array['element']['softkey_onhold']['default'] = '';

$array['element']['softkey_ringin'] = array();
$array['element']['softkey_ringin']['default'] = '';

$array['element']['softkey_offhook'] = array();
$array['element']['softkey_offhook']['default'] = '';

$array['element']['softkey_conntrans'] = array();
$array['element']['softkey_conntrans']['default'] = '';

$array['element']['softkey_digitsfoll'] = array();
$array['element']['softkey_digitsfoll']['default'] = '';

$array['element']['softkey_connconf'] = array();
$array['element']['softkey_connconf']['default'] = '';

$array['element']['softkey_ringout'] = array();
$array['element']['softkey_ringout']['default'] = '';

$array['element']['softkey_offhookfeat'] = array();
$array['element']['softkey_offhookfeat']['default'] = '';

$array['element']['softkey_onhint'] = array();
$array['element']['softkey_onhint']['default'] = '';

$array['element']['defaultline'] = array();
$array['element']['defaultline']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['name']             = array('set' => false, 'chk' => 2,
	'regexp' => '/^[a-z0-9#\*\.\/_@:-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['devicetype']       = array('set' => false, 'chk'	=> 2,
	'key' => array('12', '30', '7902', '7905', '7906', '7910', '7911', '7912', '7935', '7936', '7937', '7920', '7921', '7931', '7940', '7941', '7942', '7945', '7960', '7961', '7962', '7965', '7970', '7971', '7975', '7985', 'ata', 'kirk', 'cipc', 'nokia-icc'));
$array['filter']['keepalive']        = array('set' => false, 'chk'	=> 2,
	'cast' => 'uint','between' => array(0,999));
$array['filter']['tzoffset']         = array('set' => false, 'chk'	=> 2,
	'key' => array('-12', '-11', '-10', '-9', '-8', '-7', '-6', '-5', '-4', '-3', '-2', '-1', '0', '+1', '+2', '+3', '+4', '+5', '+6', '+7', '+8', '+9', '+10', '+11', '+12'));
$array['filter']['dtmfmode']         = array('set' => false, 'chk'	=> 2,
	'key' => array('outofband', 'inband'));
$array['filter']['transfer']         = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['park']             = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['cfwdall']          = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['cfwdbusy']         = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['cfwdbnoanswer']    = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['mwilamp']          = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off', 'wink', 'flash', 'blink'));
$array['filter']['mwioncall']        = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['dnd']              = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off', 'reject', 'silent'));
$array['filter']['pickupexten']      = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['pickupcontext']    = array('set' => false, 'chk'	=> 2,
	'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['pickupmodeanswer'] = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['permit']           = array('set' => false, 'chk'	=> 2,
	'callback'		=> 'dwho_network::valid_subnet_ipv4',
	'callbackarg'		=> array(null,DWHO_NETWORK_MASK_IPV4_DOTDEC));
$array['filter']['deny']             = array('set' => false, 'chk'	=> 2,
	'callback'		=> 'dwho_network::valid_subnet_ipv4',
	'callbackarg'		=> array(null,DWHO_NETWORK_MASK_IPV4_DOTDEC));
$array['filter']['addons']           = array('set' => false, 'chk'	=> 2,
	'maxlen' => 25);
$array['filter']['imageversion']     = array('set' => false, 'chk'	=> 2,
	'maxlength' => 20);
$array['filter']['trustphoneip']     = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['nat']              = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['directrtp']        = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['earlyrtp']         = array('set' => false, 'chk'	=> 2,
	'key' => array('none', 'offhook', 'dial', 'ringout'));
$array['filter']['private']          = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off'));
$array['filter']['privacy']          = array('set' => false, 'chk'	=> 2,
	'key' => array('on', 'off', 'full'));
$array['filter']['protocol']         = array('set' => false);
$array['filter']['softkey_onhook']        = array('set' => false, 'chk' => 2);
$array['filter']['softkey_connected']     = array('set' => false, 'chk' => 2);
$array['filter']['softkey_onhold']        = array('set' => false, 'chk' => 2);
$array['filter']['softkey_ringin']        = array('set' => false, 'chk' => 2);
$array['filter']['softkey_offhook']       = array('set' => false, 'chk' => 2);
$array['filter']['softkey_conntrans']     = array('set' => false, 'chk' => 2);
$array['filter']['softkey_digitsfoll']    = array('set' => false, 'chk' => 2);
$array['filter']['softkey_connconf']      = array('set' => false, 'chk' => 2);
$array['filter']['softkey_ringout']       = array('set' => false, 'chk' => 2);
$array['filter']['softkey_offhookfeat']   = array('set' => false, 'chk' => 2);
$array['filter']['softkey_onhint']        = array('set' => false, 'chk' => 2);
$array['filter']['defaultline']      = array('set' => false);

