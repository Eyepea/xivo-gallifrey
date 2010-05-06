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

#$array['element']['description'] = array();
#$array['element']['description']['default'] = '';

$array['element']['devicetype'] = array();
$array['element']['devicetype']['default'] = '';
#$array['element']['']['value'] = array('full');

$array['element']['tzoffset'] = array();
$array['element']['tzoffset']['default'] = '';

$array['element']['dtmfmode'] = array();
$array['element']['dtmfmode']['default'] = '';
$array['element']['dtmfmode']['value'] = array('outofband', 'inband');

$array['element']['mwilamp'] = array();
$array['element']['mwilamp']['default'] = '';
$array['element']['mwilamp']['value'] = array('on', 'off');

$array['element']['mwioncall'] = array();
$array['element']['mwioncall']['default'] = '';
$array['element']['mwioncall']['value'] = array('on', 'off');

$array['element']['pickupexten'] = array();
$array['element']['pickupexten']['default'] = '';
$array['element']['pickupexten']['value'] = array('on', 'off');

$array['element']['pickupmodeanswer'] = array();
$array['element']['pickupmodeanswer']['default'] = '';
$array['element']['pickupmodeanswer']['value'] = array('on', 'off');

$array['element']['privacy'] = array();
$array['element']['privacy']['default'] = '';
$array['element']['privacy']['value'] = array('full');

$array['element']['permit'] = array();
$array['element']['permit']['default'] = '';

$array['element']['deny'] = array();
$array['element']['deny']['default'] = '';

$array['element']['protocol'] = array();
$array['element']['protocol']['default'] = XIVO_SRE_IPBX_AST_PROTO_SCCP;
$array['element']['protocol']['forcedef'] = true;

$array['element']['defaultline'] = array();
$array['element']['defaultline']['default'] = '';

$array['element']['addons'] = array();
$array['element']['addons']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] = array('set' => false, 'chk'	=> 2);
#$array['filter']['description'] = array('set' => false, 'chk'	=> 2);
$array['filter']['devicetype'] = array('set' => false, 'chk'	=> 2);
$array['filter']['tzoffset'] = array('set' => false, 'chk'	=> 2);
$array['filter']['dtmfmode'] = array('set' => false, 'chk'	=> 2);
$array['filter']['mwilamp'] = array('set' => false, 'chk'	=> 2);
$array['filter']['mwioncall'] = array('set' => false, 'chk'	=> 2);
$array['filter']['pickupexten'] = array('set' => false, 'chk'	=> 2);
$array['filter']['pickupmodeanswer'] = array('set' => false, 'chk'	=> 2);
$array['filter']['privacy'] = array('set' => false, 'chk'	=> 2);
$array['filter']['permit'] = array('set' => false, 'chk'	=> 2);
$array['filter']['deny'] = array('set' => false, 'chk'	=> 2);
$array['filter']['defaultline'] = array('set' => false, 'chk'	=> 2);
$array['filter']['protocol'] = array('set' => false);

