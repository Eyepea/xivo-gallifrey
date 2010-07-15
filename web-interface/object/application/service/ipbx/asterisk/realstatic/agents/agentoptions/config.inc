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

$array['element']['ackcall'] = array();
$array['element']['ackcall']['value'] = array('no','yes','always');
$array['element']['ackcall']['default'] = 'no';

$array['element']['endcall'] = array();
$array['element']['endcall']['value'] = array('no','yes');
$array['element']['endcall']['default'] = 'yes';
$array['element']['endcall']['set'] = true;

$array['element']['autologoff'] = array();
$array['element']['autologoff']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['autologoff']['default'] = 0;
$array['element']['autologoff']['setdef'] = true;

$array['element']['autologoffunavail'] = array();
$array['element']['autologoffunavail']['value'] = array('no','yes');
$array['element']['autologoffunavail']['default'] = 'no';
$array['element']['autologoffunavail']['set'] = true;

$array['element']['wrapuptime'] = array();
$array['element']['wrapuptime']['value'] = array(0,5000,10000,15000,20000,
						 25000,30000,35000,40000,
						 45000,50000,55000,60000);
$array['element']['wrapuptime']['default'] = 0;
$array['element']['wrapuptime']['setdef'] = true;

$array['element']['maxlogintries'] = array();
$array['element']['maxlogintries']['value'] = range(0,10);
$array['element']['maxlogintries']['default'] = 3;
$array['element']['maxlogintries']['setdef'] = true;

$array['element']['goodbye'] = array();
$array['element']['goodbye']['default'] = 'vm-goodbye';
$array['element']['goodbye']['setdef'] = true;

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = XIVO_SRE_IPBX_AST_MOH_DEFAULT;

$array['element']['updatecdr'] = array();
$array['element']['updatecdr']['value'] = array('no','yes');
$array['element']['updatecdr']['default'] = 'no';
$array['element']['updatecdr']['set'] = true;

#$array['element']['recordagentcalls'] = array();
#$array['element']['recordagentcalls']['value'] = array('no','yes');
#$array['element']['recordagentcalls']['default'] = 'no';
#$array['element']['recordagentcalls']['set'] = true;

#$array['element']['recordformat'] = array();
#$array['element']['recordformat']['value'] = array(XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
#						   XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
#						   XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM);
#$array['element']['recordformat']['default'] = XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV;
#$array['element']['recordformat']['setdef'] = true;

$array['element']['urlprefix'] = array();
$array['element']['urlprefix']['default'] = '';

$array['element']['custom_beep'] = array();
$array['element']['custom_beep']['default'] = 'beep';
$array['element']['custom_beep']['setdef'] = true;

$array['filter'] = array();
$array['filter']['ackcall'] = array('key' => array('no','yes','always'));
$array['filter']['endcall'] = array('bool' => true);
$array['filter']['autologoff'] = array('cast' => 'uint','between' => array(0,60,5));
$array['filter']['autologoffunavail'] = array('bool' => true);
$array['filter']['wrapuptime'] = array('key' => array(0,5000,10000,15000,20000,
						      25000,30000,35000,40000,
						      45000,50000,55000,60000));
$array['filter']['maxlogintries'] = array('cast' => 'uint','between' => array(0,10));
$array['filter']['goodbye'] = array('set'	=> false,
				    'chk'	=> 2,
				    'maxlen'	=> 255,
				    'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.'$/');
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['updatecdr'] = array('bool' => true);
#$array['filter']['recordagentcalls'] = array('bool' => true);
#$array['filter']['recordformat'] = array('set'	=> false,
#					 'chk'	=> 2,
#					 'key'	=> array(XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
#							 XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
#							 XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM));
$array['filter']['urlprefix'] = array('set'	=> false,
				      'chk'	=> 2,
				      'minlen'	=> 3,
				      'maxlen'	=> 255,
				      'cast'	=> 'sgraph');
$array['filter']['custom_beep'] = array('set'		=> false,
					'chk'		=> 2,
					'maxlen'	=> 255,
					'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.'$/');

?>
