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

$array['actiontype'] = array();
$array['actiontype']['voicemenuflow'] = array('endcall',
					      'user',
					      'group',
					      'queue',
					      'meetme',
					      'voicemail',
					      'schedule',
					      'voicemenu',
					      'extension',
					      'application',
					      'ipbxapplication');
$array['actiontype']['voicemenuevent'] = array('none',
					       'endcall',
					       'user',
					       'group',
					       'queue',
					       'meetme',
					       'voicemail',
					       'schedule',
					       'voicemenu',
					       'extension',
					       'application',
					       'sound');

$array['voicemenuevent'] = array();
$array['voicemenuevent']['value'] = array('0',
					  '1',
					  '2',
					  '3',
					  '4',
					  '5',
					  '6',
					  '7',
					  '8',
					  '9',
					  '*',
					  '#',
					  'a',
					  'o',
					  't',
					  'T',
					  'i',
					  'h');
$array['voicemenuevent']['default'] = '0';

$array['defapplication'] = array();
$array['defapplication']['macro|endcall|hangup'] = 'hangup';
$array['defapplication']['macro|endcall|busy'] = 'busy';
$array['defapplication']['macro|endcall|congestion'] = 'congestion';
$array['defapplication']['macro|user'] = 'user';
$array['defapplication']['macro|group'] = 'group';
$array['defapplication']['macro|queue'] = 'queue';
$array['defapplication']['macro|meetme'] = 'meetme';
$array['defapplication']['macro|voicemail'] = 'voicemail';
$array['defapplication']['macro|schedule'] = 'schedule';
$array['defapplication']['macro|voicemenu'] = 'voicemenu';
$array['defapplication']['macro|extension'] = 'extension';
$array['defapplication']['macro|callbackdisa'] = 'callbackdisa';
$array['defapplication']['macro|disa'] = 'disa';
$array['defapplication']['macro|directory'] = 'directory';
$array['defapplication']['macro|faxtomail'] = 'faxtomail';
$array['defapplication']['macro|vmauthenticate'] = 'vmauthenticate';
$array['defapplication']['macro|voicemailmain'] = 'voicemailmain';
$array['defapplication']['macro|playsound'] = 'sound';

$array['ipbxapplication'] = array();

// ANSWER
$array['ipbxapplication']['answer'] = array();
$array['ipbxapplication']['answer']['category'] = 'answer';
$array['ipbxapplication']['answer']['arg'] = array();
$array['ipbxapplication']['answer']['arg']['delay'] = array();
$array['ipbxapplication']['answer']['arg']['delay']['default'] = '';

// AUTHENTICATION
$array['ipbxapplication']['authenticate'] = array();
$array['ipbxapplication']['authenticate']['category'] = 'authentication';
$array['ipbxapplication']['authenticate']['arg'] = array();
$array['ipbxapplication']['authenticate']['arg']['password'] = array();
$array['ipbxapplication']['authenticate']['arg']['password']['default'] = '';
$array['ipbxapplication']['authenticate']['arg']['passwordinterpreter'] = array();
$array['ipbxapplication']['authenticate']['arg']['passwordinterpreter']['value'] = array('astdb','file');
$array['ipbxapplication']['authenticate']['arg']['passwordinterpreter']['default'] = '';
$array['ipbxapplication']['authenticate']['arg']['a'] = array();
$array['ipbxapplication']['authenticate']['arg']['a']['default'] = 0;
$array['ipbxapplication']['authenticate']['arg']['j'] = array();
$array['ipbxapplication']['authenticate']['arg']['j']['default'] = 0;
$array['ipbxapplication']['authenticate']['arg']['r'] = array();
$array['ipbxapplication']['authenticate']['arg']['r']['default'] = 0;

$array['ipbxapplication']['vmauthenticate'] = array();
$array['ipbxapplication']['vmauthenticate']['category'] = 'authentication';
$array['ipbxapplication']['vmauthenticate']['arg'] = array();
$array['ipbxapplication']['vmauthenticate']['arg']['mailbox'] = array();
$array['ipbxapplication']['vmauthenticate']['arg']['mailbox']['default'] = '';
$array['ipbxapplication']['vmauthenticate']['arg']['s'] = array();
$array['ipbxapplication']['vmauthenticate']['arg']['s']['default'] = 0;

// EXECUTE
$array['ipbxapplication']['macro'] = array();
$array['ipbxapplication']['macro']['category'] = 'execute';
$array['ipbxapplication']['macro']['arg'] = array();
$array['ipbxapplication']['macro']['arg']['macroname'] = array();
$array['ipbxapplication']['macro']['arg']['macroname']['default'] = '';
$array['ipbxapplication']['macro']['arg']['args'] = array();
$array['ipbxapplication']['macro']['arg']['args']['default'] = '';

$array['ipbxapplication']['agi'] = array();
$array['ipbxapplication']['agi']['category'] = 'execute';
$array['ipbxapplication']['agi']['arg'] = array();
$array['ipbxapplication']['agi']['arg']['command'] = array();
$array['ipbxapplication']['agi']['arg']['command']['default'] = '';
$array['ipbxapplication']['agi']['arg']['args'] = array();
$array['ipbxapplication']['agi']['arg']['args']['default'] = '';

// JUMP
$array['ipbxapplication']['goto'] = array();
$array['ipbxapplication']['goto']['category'] = 'jump';
$array['ipbxapplication']['goto']['arg'] = array();
$array['ipbxapplication']['goto']['arg']['context'] = array();
$array['ipbxapplication']['goto']['arg']['context']['default'] = '';
$array['ipbxapplication']['goto']['arg']['exten'] = array();
$array['ipbxapplication']['goto']['arg']['exten']['default'] = '';
$array['ipbxapplication']['goto']['arg']['priority'] = array();
$array['ipbxapplication']['goto']['arg']['priority']['default'] = '';

$array['ipbxapplication']['gotoif'] = array();
$array['ipbxapplication']['gotoif']['category'] = 'jump';
$array['ipbxapplication']['gotoif']['arg'] = array();
$array['ipbxapplication']['gotoif']['arg']['condition'] = array();
$array['ipbxapplication']['gotoif']['arg']['condition']['default'] = '';
$array['ipbxapplication']['gotoif']['arg']['iftrue'] = array();
$array['ipbxapplication']['gotoif']['arg']['iftrue']['default'] = '';
$array['ipbxapplication']['gotoif']['arg']['iffalse'] = array();
$array['ipbxapplication']['gotoif']['arg']['iffalse']['default'] = '';

// RECORDING
$array['ipbxapplication']['mixmonitor'] = array();
$array['ipbxapplication']['mixmonitor']['category'] = 'recording';
$array['ipbxapplication']['mixmonitor']['arg'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['filename'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['filename']['default'] = '';
$array['ipbxapplication']['mixmonitor']['arg']['fileformat'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['fileformat']['default'] = XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV;
$array['ipbxapplication']['mixmonitor']['arg']['fileformat']['value'] = array(
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN);
$array['ipbxapplication']['mixmonitor']['arg']['a'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['a']['default'] = 0;
$array['ipbxapplication']['mixmonitor']['arg']['b'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['b']['default'] = 0;
$array['ipbxapplication']['mixmonitor']['arg']['v'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['v']['default'] = 0;
$array['ipbxapplication']['mixmonitor']['arg']['v_volume'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['v_volume']['default'] = 1;
$array['ipbxapplication']['mixmonitor']['arg']['v_volume']['value'] = array(4,3,2,1,0,-1,-2,-3,-4);
$array['ipbxapplication']['mixmonitor']['arg']['V'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['V']['default'] = 0;
$array['ipbxapplication']['mixmonitor']['arg']['V_volume'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['V_volume']['default'] = 1;
$array['ipbxapplication']['mixmonitor']['arg']['V_volume']['value'] = array(4,3,2,1,0,-1,-2,-3,-4);
$array['ipbxapplication']['mixmonitor']['arg']['W'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['W']['default'] = 0;
$array['ipbxapplication']['mixmonitor']['arg']['W_volume'] = array();
$array['ipbxapplication']['mixmonitor']['arg']['W_volume']['default'] = 1;
$array['ipbxapplication']['mixmonitor']['arg']['W_volume']['value'] = array(4,3,2,1,0,-1,-2,-3,-4);

$array['ipbxapplication']['monitor'] = array();
$array['ipbxapplication']['monitor']['category'] = 'recording';
$array['ipbxapplication']['monitor']['arg'] = array();
$array['ipbxapplication']['monitor']['arg']['fileformat'] = array();
$array['ipbxapplication']['monitor']['arg']['fileformat']['default'] = XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV;
$array['ipbxapplication']['monitor']['arg']['fileformat']['value'] = array(
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN);
$array['ipbxapplication']['monitor']['arg']['basename'] = array();
$array['ipbxapplication']['monitor']['arg']['basename']['default'] = '';
$array['ipbxapplication']['monitor']['arg']['m'] = array();
$array['ipbxapplication']['monitor']['arg']['m']['default'] = 0;
$array['ipbxapplication']['monitor']['arg']['b'] = array();
$array['ipbxapplication']['monitor']['arg']['b']['default'] = 0;

$array['ipbxapplication']['record'] = array();
$array['ipbxapplication']['record']['category'] = 'recording';
$array['ipbxapplication']['record']['arg'] = array();
$array['ipbxapplication']['record']['arg']['filename'] = array();
$array['ipbxapplication']['record']['arg']['filename']['default'] = '';
$array['ipbxapplication']['record']['arg']['fileformat'] = array();
$array['ipbxapplication']['record']['arg']['fileformat']['default'] = XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV;
$array['ipbxapplication']['record']['arg']['fileformat']['value'] = array(
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN);
$array['ipbxapplication']['record']['arg']['silence'] = array();
$array['ipbxapplication']['record']['arg']['silence']['default'] = '';
$array['ipbxapplication']['record']['arg']['maxduration'] = array();
$array['ipbxapplication']['record']['arg']['maxduration']['default'] = '';
$array['ipbxapplication']['record']['arg']['a'] = array();
$array['ipbxapplication']['record']['arg']['a']['default'] = 0;
$array['ipbxapplication']['record']['arg']['n'] = array();
$array['ipbxapplication']['record']['arg']['n']['default'] = 0;
$array['ipbxapplication']['record']['arg']['q'] = array();
$array['ipbxapplication']['record']['arg']['q']['default'] = 0;
$array['ipbxapplication']['record']['arg']['s'] = array();
$array['ipbxapplication']['record']['arg']['s']['default'] = 0;
$array['ipbxapplication']['record']['arg']['t'] = array();
$array['ipbxapplication']['record']['arg']['t']['default'] = 0;

$array['ipbxapplication']['stopmonitor'] = array();
$array['ipbxapplication']['stopmonitor']['category'] = 'recording';
$array['ipbxapplication']['stopmonitor']['arg'] = null;

// SOUND
$array['ipbxapplication']['background'] = array();
$array['ipbxapplication']['background']['category'] = 'sound';
$array['ipbxapplication']['background']['arg'] = array();
$array['ipbxapplication']['background']['arg']['filename'] = array();
$array['ipbxapplication']['background']['arg']['filename']['default'] = '';
$array['ipbxapplication']['background']['arg']['s'] = array();
$array['ipbxapplication']['background']['arg']['s']['default'] = 0;
$array['ipbxapplication']['background']['arg']['n'] = array();
$array['ipbxapplication']['background']['arg']['n']['default'] = 0;
$array['ipbxapplication']['background']['arg']['m'] = array();
$array['ipbxapplication']['background']['arg']['m']['default'] = 0;
$array['ipbxapplication']['background']['arg']['language'] = array();
$array['ipbxapplication']['background']['arg']['language']['default'] = '';
$array['ipbxapplication']['background']['arg']['context'] = array();
$array['ipbxapplication']['background']['arg']['context']['default'] = '';

$array['ipbxapplication']['playback'] = array();
$array['ipbxapplication']['playback']['category'] = 'sound';
$array['ipbxapplication']['playback']['arg'] = array();
$array['ipbxapplication']['playback']['arg']['filename'] = array();
$array['ipbxapplication']['playback']['arg']['filename']['default'] = '';
$array['ipbxapplication']['playback']['arg']['skip'] = array();
$array['ipbxapplication']['playback']['arg']['skip']['default'] = 0;
$array['ipbxapplication']['playback']['arg']['noanswer'] = array();
$array['ipbxapplication']['playback']['arg']['noanswer']['default'] = 0;
$array['ipbxapplication']['playback']['arg']['j'] = array();
$array['ipbxapplication']['playback']['arg']['j']['default'] = 0;

// TIMEOUT
$array['ipbxapplication']['absolutetimeout'] = array();
$array['ipbxapplication']['absolutetimeout']['category'] = 'timeout';
$array['ipbxapplication']['absolutetimeout']['arg'] = array();
$array['ipbxapplication']['absolutetimeout']['arg']['timeout'] = array();
$array['ipbxapplication']['absolutetimeout']['arg']['timeout']['default'] = '';

$array['ipbxapplication']['digittimeout'] = array();
$array['ipbxapplication']['digittimeout']['category'] = 'timeout';
$array['ipbxapplication']['digittimeout']['arg'] = array();
$array['ipbxapplication']['digittimeout']['arg']['timeout'] = array();
$array['ipbxapplication']['digittimeout']['arg']['timeout']['default'] = '';

$array['ipbxapplication']['responsetimeout'] = array();
$array['ipbxapplication']['responsetimeout']['category'] = 'timeout';
$array['ipbxapplication']['responsetimeout']['arg'] = array();
$array['ipbxapplication']['responsetimeout']['arg']['timeout'] = array();
$array['ipbxapplication']['responsetimeout']['arg']['timeout']['default'] = '';

// VARIABLE
$array['ipbxapplication']['read'] = array();
$array['ipbxapplication']['read']['category'] = 'variable';
$array['ipbxapplication']['read']['arg'] = array();
$array['ipbxapplication']['read']['arg']['variable'] = array();
$array['ipbxapplication']['read']['arg']['variable']['default'] = '';
$array['ipbxapplication']['read']['arg']['filename'] = array();
$array['ipbxapplication']['read']['arg']['filename']['default'] = '';
$array['ipbxapplication']['read']['arg']['maxdigits'] = array();
$array['ipbxapplication']['read']['arg']['maxdigits']['default'] = '';
$array['ipbxapplication']['read']['arg']['s'] = array();
$array['ipbxapplication']['read']['arg']['s']['default'] = 0;
$array['ipbxapplication']['read']['arg']['i'] = array();
$array['ipbxapplication']['read']['arg']['i']['default'] = 0;
$array['ipbxapplication']['read']['arg']['n'] = array();
$array['ipbxapplication']['read']['arg']['n']['default'] = 0;
$array['ipbxapplication']['read']['arg']['attempts'] = array();
$array['ipbxapplication']['read']['arg']['attempts']['default'] = '';
$array['ipbxapplication']['read']['arg']['timeout'] = array();
$array['ipbxapplication']['read']['arg']['timeout']['default'] = '';

$array['ipbxapplication']['set'] = array();
$array['ipbxapplication']['set']['category'] = 'variable';
$array['ipbxapplication']['set']['arg'] = array();
$array['ipbxapplication']['set']['arg']['name'] = array();
$array['ipbxapplication']['set']['arg']['name']['default'] = '';
$array['ipbxapplication']['set']['arg']['value'] = array();
$array['ipbxapplication']['set']['arg']['value']['default'] = '';
$array['ipbxapplication']['set']['arg']['g'] = array();
$array['ipbxapplication']['set']['arg']['g']['default'] = 0;

$array['ipbxapplication']['setcallerid'] = array();
$array['ipbxapplication']['setcallerid']['category'] = 'variable';
$array['ipbxapplication']['setcallerid']['arg'] = array();
$array['ipbxapplication']['setcallerid']['arg']['callerid'] = array();
$array['ipbxapplication']['setcallerid']['arg']['callerid']['default'] = '';

$array['ipbxapplication']['setcidname'] = array();
$array['ipbxapplication']['setcidname']['category'] = 'variable';
$array['ipbxapplication']['setcidname']['arg'] = array();
$array['ipbxapplication']['setcidname']['arg']['name'] = array();
$array['ipbxapplication']['setcidname']['arg']['name']['default'] = '';

$array['ipbxapplication']['setcidnum'] = array();
$array['ipbxapplication']['setcidnum']['category'] = 'variable';
$array['ipbxapplication']['setcidnum']['arg'] = array();
$array['ipbxapplication']['setcidnum']['arg']['number'] = array();
$array['ipbxapplication']['setcidnum']['arg']['number']['default'] = '';

$array['ipbxapplication']['setlanguage'] = array();
$array['ipbxapplication']['setlanguage']['category'] = 'variable';
$array['ipbxapplication']['setlanguage']['arg'] = array();
$array['ipbxapplication']['setlanguage']['arg']['language'] = array();
$array['ipbxapplication']['setlanguage']['arg']['language']['default'] = '';

$array['ipbxapplication']['noop'] = array();
$array['ipbxapplication']['noop']['category'] = 'variable';
$array['ipbxapplication']['noop']['arg'] = array();
$array['ipbxapplication']['noop']['arg']['data'] = array();
$array['ipbxapplication']['noop']['arg']['data']['default'] = '';

// WAIT
$array['ipbxapplication']['wait'] = array();
$array['ipbxapplication']['wait']['category'] = 'wait';
$array['ipbxapplication']['wait']['arg'] = array();
$array['ipbxapplication']['wait']['arg']['seconds'] = array();
$array['ipbxapplication']['wait']['arg']['seconds']['default'] = '';

$array['ipbxapplication']['waitexten'] = array();
$array['ipbxapplication']['waitexten']['category'] = 'wait';
$array['ipbxapplication']['waitexten']['arg'] = array();
$array['ipbxapplication']['waitexten']['arg']['seconds'] = array();
$array['ipbxapplication']['waitexten']['arg']['seconds']['default'] = '';
$array['ipbxapplication']['waitexten']['arg']['m'] = array();
$array['ipbxapplication']['waitexten']['arg']['m']['default'] = 0;
$array['ipbxapplication']['waitexten']['arg']['musiconhold'] = array();
$array['ipbxapplication']['waitexten']['arg']['musiconhold']['default'] = '';

$array['ipbxapplication']['waitforring'] = array();
$array['ipbxapplication']['waitforring']['category'] = 'wait';
$array['ipbxapplication']['waitforring']['arg'] = array();
$array['ipbxapplication']['waitforring']['arg']['timeout'] = array();
$array['ipbxapplication']['waitforring']['arg']['timeout']['default'] = '';

$array['ipbxapplication']['waitmusiconhold'] = array();
$array['ipbxapplication']['waitmusiconhold']['category'] = 'wait';
$array['ipbxapplication']['waitmusiconhold']['arg'] = array();
$array['ipbxapplication']['waitmusiconhold']['arg']['delay'] = array();
$array['ipbxapplication']['waitmusiconhold']['arg']['delay']['default'] = '';

?>
