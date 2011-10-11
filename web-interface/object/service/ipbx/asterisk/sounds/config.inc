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

$array['option']['defdir'] = array();
$array['option']['defdir']['acd'] = 1;
$array['option']['defdir']['beep'] = 1;
$array['option']['defdir']['features'] = 1;
$array['option']['defdir']['goodbye'] = 1;
$array['option']['defdir']['monitor'] = 1;
$array['option']['defdir']['playback'] = 1;
$array['option']['defdir']['recordings'] = 1;

$array['option']['dir'] = array();
$array['option']['dir']['name'] = '/^[a-z0-9_-]{1,200}$/i';

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

$array['filter'] = array();
$array['filter']['filename'] = array('regexp' => '/^['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
						 XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH.'\.[a-zA-Z0-9]{2,5}$/','minlen' => 4,'maxlen' => 200);
$array['filter']['dirname'] = array('regexp' => '/^[a-z0-9_-]+$/i','minlen' => 1,'maxlen' => 200);

?>
