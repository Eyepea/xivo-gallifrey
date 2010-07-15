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

$array['element']['persistentagents'] = array();
$array['element']['persistentagents']['value'] = array('no','yes');
$array['element']['persistentagents']['default'] = 'yes';
$array['element']['persistentagents']['set'] = true;

$array['element']['multiplelogin'] = array();
$array['element']['multiplelogin']['value'] = array('no','yes');
$array['element']['multiplelogin']['default'] = 'yes';
$array['element']['multiplelogin']['set'] = true;

$array['element']['recordagentcalls'] = array();
$array['element']['recordagentcalls']['value'] = array('no','yes');
$array['element']['recordagentcalls']['default'] = 'no';
$array['element']['recordagentcalls']['set'] = true;

$array['element']['recordformat'] = array();
$array['element']['recordformat']['value'] = array(XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
						   XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
						   XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM);
$array['element']['recordformat']['default'] = XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV;
$array['element']['recordformat']['setdef'] = true;

$array['filter'] = array();
$array['filter']['persistentagents'] = array('bool' => true);
$array['filter']['multiplelogin'] = array('bool' => true);
$array['filter']['recordagentcalls'] = array('bool' => true);
$array['filter']['recordformat'] = array('set'	=> false,
					 'chk'	=> 2,
					 'key'	=> array(XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
							 XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
							 XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM));


?>
