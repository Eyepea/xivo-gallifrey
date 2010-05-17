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
$array['element']['dnd']['value'] = array('on', 'off');

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

$array['element']['deny'] = array();
$array['element']['deny']['default'] = '';

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

$array['element']['defaultline'] = array();
$array['element']['defaultline']['default'] = '';



$array['filter'] = array();
$array['filter']['name']             = array('set' => false, 'chk'	=> 2);
$array['filter']['devicetype']       = array('set' => false, 'chk'	=> 2);
$array['filter']['keepalive']        = array('set' => false, 'chk'	=> 2);
$array['filter']['tzoffset']         = array('set' => false, 'chk'	=> 2);
$array['filter']['dtmfmode']         = array('set' => false, 'chk'	=> 2);
$array['filter']['transfer']         = array('set' => false, 'chk'	=> 2);
$array['filter']['park']             = array('set' => false, 'chk'	=> 2);
$array['filter']['cfwdall']          = array('set' => false, 'chk'	=> 2);
$array['filter']['cfwdbusy']         = array('set' => false, 'chk'	=> 2);
$array['filter']['cfwdbnoanswer']    = array('set' => false, 'chk'	=> 2);
$array['filter']['mwilamp']          = array('set' => false, 'chk'	=> 2);
$array['filter']['mwioncall']        = array('set' => false, 'chk'	=> 2);
$array['filter']['dnd']              = array('set' => false, 'chk'	=> 2);
$array['filter']['pickupexten']      = array('set' => false, 'chk'	=> 2);
$array['filter']['pickupcontext']    = array('set' => false, 'chk'	=> 2);
$array['filter']['pickupmodeanswer'] = array('set' => false, 'chk'	=> 2);
$array['filter']['permit']           = array('set' => false, 'chk'	=> 2);
$array['filter']['deny']             = array('set' => false, 'chk'	=> 2);
$array['filter']['addons']           = array('set' => false, 'chk'	=> 2);
$array['filter']['imageversion']     = array('set' => false, 'chk'	=> 2);
$array['filter']['trustphoneip']     = array('set' => false, 'chk'	=> 2);
$array['filter']['nat']              = array('set' => false, 'chk'	=> 2);
$array['filter']['directrtp']        = array('set' => false, 'chk'	=> 2);
$array['filter']['earlyrtp']         = array('set' => false, 'chk'	=> 2);
$array['filter']['private']          = array('set' => false, 'chk'	=> 2);
$array['filter']['privacy']          = array('set' => false, 'chk'	=> 2);
$array['filter']['protocol']         = array('set' => false, 'chk'	=> 2);
$array['filter']['defaultline']      = array('set' => false, 'chk'	=> 2);

