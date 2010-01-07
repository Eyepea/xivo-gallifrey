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

$array['option'] = array();
$array['option']['dir'] = array();
$array['option']['dir']['name'] = '/^[a-z0-9_-]{1,20}$/';

$array['option']['file'] = array();
$array['option']['file']['name'] = '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
				    XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.'\.[a-zA-Z0-9]{2,5}$/';
$array['option']['file']['extension'] = array();
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN] = true;
$array['option']['file']['extension'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_MP3] = true;

$array['element'] = array();

$array['element']['category'] = array();
$array['element']['category']['default'] = '';

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('files','custom','quietmp3','mp3','mp3nb','quietmp3nb');
$array['element']['mode']['default'] = 'files';

$array['element']['directory'] = array();
$array['element']['directory']['default'] = '';

$array['element']['application'] = array();
$array['element']['application']['default'] = '/usr/bin/madplay --mono -a -10 -R 8000 --output=raw:-';
$array['element']['application']['null'] = true;

$array['element']['random'] = array();
$array['element']['random']['value'] = array('no','yes');
$array['element']['random']['default'] = 'no';
$array['element']['random']['set'] = true;

$array['element']['filename'] = array();
$array['element']['filename']['unset'] = true;

$array['filter'] = array();
$array['filter']['category'] = array('nokey' => array('general'),'regexp' => '/^[a-z0-9_-]+$/','minlen' => 1,'maxlen' => 20);
$array['filter']['mode'] = array('key' => array('files','custom','quietmp3','mp3','mp3nb','quietmp3nb'));
$array['filter']['application'] = array('set' => false,'chk' => 2,'maxlen' => 128);
$array['filter']['random'] = array('bool' => true);
$array['filter']['filename'] = array('set' => false,'regexp' => '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
								XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.'\.[a-zA-Z0-9]{2,5}$/');

?>
