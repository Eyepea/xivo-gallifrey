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

$array['element']['servername'] = array();
$array['element']['servername']['default'] = 'Asterisk';

$array['element']['keepalive'] = array();
$array['element']['keepalive']['default'] = 60;

$array['element']['debug'] = array();
$array['element']['debug']['default'] = 'core';

$array['element']['context'] = array();
$array['element']['context']['default'] = 'sccp';

$array['element']['dateFormat'] = array();
$array['element']['dateFormat']['default'] = 'D.M.Y';

$array['element']['bindaddr'] = array();
$array['element']['bindaddr']['default'] = '0.0.0.0';

$array['element']['port'] = array();
$array['element']['port']['default'] = 2000;

$array['element']['disallow'] = array();
$array['element']['disallow']['default'] = 'all';
$array['element']['disallow']['value'] = array('all');

$array['element']['allow'] = array();
$array['element']['allow']['default'] = 'alaw';
$array['element']['allow']['value'] = array('alaw', 'ulaw', 'G729');

$array['element']['firstdigittimeout'] = array();
$array['element']['firstdigittimeout']['default'] = 16;

$array['element']['digittimeout'] = array();
$array['element']['digittimeout']['default'] = 8;

$array['element']['digittimeoutchar'] = array();
$array['element']['digittimeoutchar']['default'] = '#';

$array['element']['autoanswer_ring_time'] = array();
$array['element']['autoanswer_ring_time']['default'] = 0;

$array['element']['autoanswer_tone'] = array();
$array['element']['autoanswer_tone']['default'] = '0x32';

$array['element']['remotehangup_tone'] = array();
$array['element']['remotehangup_tone']['default'] = '0x32';

$array['element']['transfer_tone'] = array();
$array['element']['transfer_tone']['default'] = 0;

$array['element']['callwaiting_tone'] = array();
$array['element']['callwaiting_tone']['default'] = '0x2d';

$array['element']['musicclass'] = array();
$array['element']['musicclass']['default'] = 'default';

$array['element']['language'] = array();
$array['element']['language']['default'] = 'fr';
$array['element']['language']['value'] = array('fr', 'en');

$array['element']['callevents'] = array();
$array['element']['callevents']['default'] = 'no';
$array['element']['callevents']['value'] = array('no', 'yes');

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = 'skinny';

$array['element']['deny'] = array();
$array['element']['deny']['default'] = '0.0.0.0/0.0.0.0';

$array['element']['permit'] = array();
$array['element']['permit']['default'] = '0.0.0.0/255.255.255.0';

$array['element']['localnet'] = array();
$array['element']['localnet']['default'] = '192.168.1.0/255.255.255.0';

$array['element']['externip'] = array();
$array['element']['externip']['default'] = '1.2.3.4';

$array['element']['externhost'] = array();
$array['element']['externhost']['default'] = 'mydomain.dyndns.org';

$array['element']['externrefresh'] = array();
$array['element']['externrefresh']['default'] = 60;

$array['element']['dnd'] = array();
$array['element']['dnd']['default'] = 'off';
$array['element']['dnd']['value'] = array('off', 'on');

$array['element']['sccp_tos'] = array();
$array['element']['sccp_tos']['default'] = '0x68';

$array['element']['sccp_cos'] = array();
$array['element']['sccp_cos']['default'] = 4;

$array['element']['audio_tos'] = array();
$array['element']['audio_tos']['default'] = '0xB8';

$array['element']['audio_cos'] = array();
$array['element']['audio_cos']['default'] = 6;

$array['element']['video_tos'] = array();
$array['element']['video_tos']['default'] = '0x88';

$array['element']['video_cos'] = array();
$array['element']['video_cos']['default'] = 5;

$array['element']['echocancel'] = array();
$array['element']['echocancel']['default'] = 'on';
$array['element']['echocancel']['value'] = array('off', 'on');

$array['element']['silencesuppression'] = array();
$array['element']['silencesuppression']['default'] = 'off';
$array['element']['silencesuppression']['value'] = array('off', 'on');

$array['element']['callgroup'] = array();
$array['element']['callgroup']['default'] = '1,3-4';

$array['element']['pickupgroup'] = array();
$array['element']['pickupgroup']['default'] = '1,3-5';

$array['element']['amaflags'] = array();
$array['element']['amaflags']['default'] = '';

$array['element']['trustphoneip'] = array();
$array['element']['trustphoneip']['default'] = 'no';
$array['element']['trustphoneip']['value'] = array('no', 'yes');

$array['element']['earlyrtp'] = array();
$array['element']['earlyrtp']['default'] = 'none';

$array['element']['private'] = array();
$array['element']['private']['default'] = 'on';
$array['element']['private']['value'] = array('off', 'on');

$array['element']['mwilamp'] = array();
$array['element']['mwilamp']['default'] = 'on';
$array['element']['mwilamp']['value'] = array('off', 'on');

$array['element']['mwioncall'] = array();
$array['element']['mwioncall']['default'] = 'off';
$array['element']['mwioncall']['value'] = array('off', 'on');

$array['element']['blindtransferindication'] = array();
$array['element']['blindtransferindication']['default'] = 'ring';

$array['element']['protocolversion'] = array();
$array['element']['protocolversion']['default'] = 11;

$array['element']['cfwdall'] = array();
$array['element']['cfwdall']['default'] = 'off';
$array['element']['cfwdall']['value'] = array('off', 'on');

$array['element']['cfwdbusy'] = array();
$array['element']['cfwdbusy']['default'] = 'off';
$array['element']['cfwdbusy']['value'] = array('off', 'on');

$array['element']['cfwdnoanswer'] = array();
$array['element']['cfwdnoanswer']['default'] = 'off';
$array['element']['cfwdnoanswer']['value'] = array('off', 'on');

$array['element']['devicetable'] = array();
$array['element']['devicetable']['default'] = 'sccpdevice';

$array['element']['linetable'] = array();
$array['element']['linetable']['default'] = 'sccpline';

$array['element']['nat'] = array();
$array['element']['nat']['default'] = 'off';
$array['element']['nat']['value'] = array('off', 'on');

$array['element']['directrtp'] = array();
$array['element']['directrtp']['default'] = 'off';
$array['element']['directrtp']['value'] = array('off', 'on');

$array['element']['allowoverlap'] = array();
$array['element']['allowoverlap']['default'] = 'off';
$array['element']['allowoverlap']['value'] = array('off', 'on');

$array['element']['callanswerorder'] = array();
$array['element']['callanswerorder']['default'] = 'olderfirst';
$array['element']['callanswerorder']['value'] = array('olderfirst', 'lastestfirst');

$array['element']['jbenable'] = array();
$array['element']['jbenable']['default'] = 'yes';
$array['element']['jbenable']['value'] = array('no', 'yes');

$array['element']['jbforce'] = array();
$array['element']['jbforce']['default'] = 'no';
$array['element']['jbforce']['value'] = array('no', 'yes');

$array['element']['jbmaxsize'] = array();
$array['element']['jbmaxsize']['default'] = 200;

$array['element']['jbresyncthreshold'] = array();
$array['element']['jbresyncthreshold']['default'] = 1000;

$array['element']['jbimpl'] = array();
$array['element']['jbimpl']['default'] = 'fixed';

$array['element']['jblog'] = array();
$array['element']['jblog']['default'] = 'no';
$array['element']['jblog']['value'] = array('no', 'yes');

$array['element']['hotline_enabled'] = array();
$array['element']['hotline_enabled']['default'] = 'yes';
$array['element']['hotline_enabled']['value'] = array('no', 'yes');

$array['element']['hotline_context'] = array();
$array['element']['hotline_context']['default'] = 'xivo-initconfig';

$array['element']['hotline_extension'] = array();
$array['element']['hotline_extension']['default'] = 'sccp';

$array['filter'] = array();
/*
$array['filter']['name'] = array('set' => false, 'chk'	=> 2);
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
*/
