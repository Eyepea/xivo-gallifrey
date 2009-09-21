<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

$array['app'] = array();

$array['app']['absolutetimeout'] = array();
$array['app']['absolutetimeout']['displayname'] = 'AbsoluteTimeout';
$array['app']['absolutetimeout']['app'] = 'Set';
$array['app']['absolutetimeout']['appdata'] = 'TIMEOUT(absolute)=%s';
$array['app']['absolutetimeout']['arg'] = array();
$array['app']['absolutetimeout']['arg'][0] = array('cast'	=> 'ufloat',
						   'between'	=> array(0,600));

$array['app']['agentcallbacklogin'] = array();
$array['app']['agentcallbacklogin']['displayname'] = 'AgentCallbackLogin';
$array['app']['agentcallbacklogin']['arg'] = array();
$array['app']['agentcallbacklogin']['arg'][0] = array('minlen'	=> 1,
						      'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN.'$/');
$array['app']['agentcallbacklogin']['arg'][1] = array('set'	=> false,
						      'chk'	=> 2,
						      'key'	=> array('s'));
$array['app']['agentcallbacklogin']['arg'][2] = array('set'	=> false,
						      'chk'	=> 2,
						      'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN.
								   '(?:'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_ATCONTEXT.')?$/');

$array['app']['agentlogin'] = array();
$array['app']['agentlogin']['displayname'] = 'AgentLogin';
$array['app']['agentlogin']['arg'] = array();
$array['app']['agentlogin']['arg'][0] = array('set'	=> false,
					      'chk'	=> 2,
					      'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN.'$/');
$array['app']['agentlogin']['arg'][1] = array('set'	=> false,
					      'chk'	=> 2,
					      'key'	=> array('s'));

$array['app']['agi'] = array();
$array['app']['agi']['displayname'] = 'AGI';
$array['app']['agi']['arg'] = array();
$array['app']['agi']['arg'][0] = array('minlen'	=> 1,
				       'notag'	=> false,
				       'cast'	=> 'sgraph',
				       'regexp'	=> '/^[^\|,]+$/');
$array['app']['agi']['arg'][1] = array('set'	=> false,
				       'chk'	=> 2);

$array['app']['answer'] = array();
$array['app']['answer']['displayname'] = 'Answer';
$array['app']['answer']['arg'] = array();
$array['app']['answer']['arg'][0] = array('set'		=> false,
					  'chk'		=> 2,
					  'cast'	=> 'uint',
					  'between'	=> array(0,60000));

$array['app']['authenticate'] = array();
$array['app']['authenticate']['displayname'] = 'Authenticate';
$array['app']['authenticate']['arg'] = array();
$array['app']['authenticate']['arg'][0] = array('minlen'	=> 1,
						'notag'		=> false,
						'cast'		=> 'sgraph',
						'regexp'	=> '/^(?:[0-9\*#]+'.
								   '|(?:[0-9\*#]*'.
									XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
									'[0-9\*#]*)+'.
								   '|\/[^\|,]+)$/');
$array['app']['authenticate']['arg'][1] = array('set'		=> false,
						'chk'		=> 2,
						'uchar'		=> array('a','d','j','m','r'),
						'strict'	=> false,
						'regexp'	=> '/^(?:[^m]*d[^m]*|[^dr]*m[^dr]*|[^mdr]*)$/');

$array['app']['background'] = array();
$array['app']['background']['displayname'] = 'BackGround';
$array['app']['background']['arg'] = array();
$array['app']['background']['arg'][0] = array('minlen'	=> 1,
					      'notag'	=> false,
					      'cast'	=> 'sgraph',
					      'regexp'	=> '/^[^\|,]+$/');
$array['app']['background']['arg'][1] = array('set'	=> false,
					      'chk'	=> 2,
					      'uchar'	=> array('s','n','m'));
$array['app']['background']['arg'][2] = array('set'	=> false,
					      'chk'	=> 2,
					      'regexp'	=> '/^(?:[a-zA-Z]{2,3}(?:[_-][a-zA-Z]{2,3})?'.
							   '|'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.')$/');
$array['app']['background']['arg'][3] = array('set'	=> false,
					      'chk'	=> 2,
					      'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_CONTEXT.'$/');

$array['app']['busy'] = array();
$array['app']['busy']['displayname'] = 'Busy';
$array['app']['busy']['arg'] = array();
$array['app']['busy']['arg'][0] = array('set'		=> false,
					'chk'		=> 2,
					'cast'		=> 'ufloat',
					'between'	=> array(0,600));

$array['app']['chanspy'] = array();
$array['app']['chanspy']['displayname'] = 'ChanSpy';
$array['app']['chanspy']['arg'] = array();
$array['app']['chanspy']['arg'][0] = array('set'	=> false,
					   'chk'	=> 2,
					   'regexp'	=> '/^(?:[a-zA-Z0-9_-]{1,32}'.
							   '|(?:[a-zA-Z0-9_-]*'.
								XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
								'[a-zA-Z0-9_-]*)+)$/');
$array['app']['chanspy']['arg'][1] = array('set'	=> false,
					   'chk'	=> 2,
					   'maxlen'	=> 127,
					   'notag'	=> false,
					   'cast'	=> 'sgraph',
					   'regexp'	=> '/^[^\|,]+/');

$array['app']['congestion'] = array();
$array['app']['congestion']['displayname'] = 'Congestion';
$array['app']['congestion']['arg'] = array();
$array['app']['congestion']['arg'][0] = array('set'	=> false,
					      'chk'	=> 2,
					      'cast'	=> 'ufloat',
					      'between'	=> array(0,600));

$array['app']['digittimeout'] = array();
$array['app']['digittimeout']['displayname'] = 'DigitTimeout';
$array['app']['digittimeout']['app'] = 'Set';
$array['app']['digittimeout']['appdata'] = 'TIMEOUT(digit)=%s';
$array['app']['digittimeout']['arg'] = array();
$array['app']['digittimeout']['arg'][0] = array('cast'		=> 'ufloat',
						'between'	=> array(0,600));

$array['app']['directory'] = array();
$array['app']['directory']['displayname'] = 'Directory';
$array['app']['directory']['arg'] = array();
$array['app']['directory']['arg'][0] = array('minlen'	=> 1,
					     'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_CONTEXT.'$/');
$array['app']['directory']['arg'][1] = array('set'	=> false,
					     'chk'	=> 2,
					     'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_CONTEXT.'$/');
$arrap['app']['directory']['arg'][2] = array('set'	=> false,
					     'chk'	=> 2,
					     'key'	=> array('f'));

$array['app']['disa'] = array();
$array['app']['disa']['displayname'] = 'DISA';
$array['app']['disa']['arg'] = array();
$array['app']['disa']['arg'][0] = array('minlen'	=> 1,
					'notag'		=> false,
					'cast'		=> 'sgraph',
					'regexp'	=> '/^[^\|,]+$/');
$array['app']['disa']['arg'][1] = array('set'		=> false,
					'chk'		=> 2,
					'notag'		=> false,
					'cast'		=> 'sgraph',
					'regexp'	=> '/^(?:'.XIVO_SRE_IPBX_AST_REGEXP_CONTEXT.
							   '|(?:[a-z0-9_-]*'.
								XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
								'[a-z0-9_-]*)+'.
							   '|[^\|,]*<[^\|,]+>)$/');
$array['app']['disa']['arg'][2] = array('set'		=> false,
					'chk'		=> 2,
					'notag'		=> false,
					'cast'		=> 'sgraph',
					'regexp'	=> '/^[^\|,]*<[^\|,]+>$/');
$array['app']['disa']['arg'][3] = array('set'		=> false,
					'chk'		=> 2,
					'regexp'	=> '/^(?:[0-9\*#]{1,40}'.
							   '|(?:[0-9\*#]*'.
								XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
								'[0-9\*#]*)+)$/');

$array['app']['goto'] = array();
$array['app']['goto']['displayname'] = 'Goto';
$array['app']['goto']['arg'] = array();
$array['app']['goto']['arg'][0] = array('set'		=> false,
					'chk'		=> 2,
					'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_CONTEXT.'$/');
$array['app']['goto']['arg'][1] = array('set'		=> false,
					'chk'		=> 2,
					'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN_EXTENDED.'$/');
$array['app']['goto']['arg'][2] = array('minlen'	=> 1,
					'regexp'	=> '/^(?:[a-zA-Z0-9_\+\-]{1,42}'.
							   '|(?:[a-zA-Z0-9_\+\-]*'.
								XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
								'[a-zA-Z0-9_\+\-]*)+)$/');

$array['app']['gotoif'] = array();
$array['app']['gotoif']['displayname'] = 'GotoIf';
$array['app']['gotoif']['arg'] = array();
$array['app']['gotoif']['arg'][0] = array('minlen'	=> 4,
					  'notag'	=> false,
					  'cast'	=> 'sgraph',
					  'regexp'	=> '/^[^\?]+\?(?:.+\:?|[^\:]*\:[^\:]+)$/');

$array['app']['hangup'] = array();
$array['app']['hangup']['displayname'] = 'Hangup';

$array['app']['macro'] = array();
$array['app']['macro']['displayname'] = 'Macro';
$array['app']['macro']['arg'] = array();
$array['app']['macro']['arg'][0] = array('minlen'	=> 1,
					 'regexp'	=> '/^(?:[a-z0-9_-]{1,80}'.
							   '|(?:[a-z0-9_-]*'.
								XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
								'[a-z0-9_-]*)+)$/');
$array['app']['macro']['arg'][1] = array('set'		=> false,
					 'chk'		=> 2);

$array['app']['mixmonitor'] = array();
$array['app']['mixmonitor']['displayname'] = 'MixMonitor';
$array['app']['mixmonitor']['arg'] = array();
$array['app']['mixmonitor']['arg'][0] = array('minlen'	=> 4,
					      'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_AUDIOFILE.'$/');
$array['app']['mixmonitor']['arg'][1] = array('set'	=> false,
					      'chk'	=> 2,
					      'uchar'	=> array('a','b','v','V','W'),
					      'strict'	=> false,
					      'regexp'	=> '/^[ab]{0,2}?(?:[vVW]\((?:0|-?[1-4])\)){0,3}[ab]{0,2}?$/');

$array['app']['monitor'] = array();
$array['app']['monitor']['displayname'] = 'Monitor';
$array['app']['monitor']['arg'] = array();
$array['app']['monitor']['arg'][0] = array('set'	=> false,
					   'chk'	=> 2,
					   'key' => array(
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM,
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG,
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW,
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW,
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU,
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX,
							XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN));
$array['app']['monitor']['arg'][1] = array('set'	=> false,
					   'chk'	=> 2,
					   'regexp'	=> '/^(?:[a-zA-Z0-9_-]{1,50}'.
							   '|(?:[a-zA-Z0-9_-]*'.
								XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
								'[a-zA-Z0-9_-]*)+)$/');
$array['app']['monitor']['arg'][2] = array('set'	=> false,
					   'chk'	=> 2,
					   'uchar'	=> array('m','b'));

$array['app']['noop'] = array();
$array['app']['noop']['displayname'] = 'NoOp';
$array['app']['noop']['arg'] = array();
$array['app']['noop']['arg'][0] = array('set'	=> false,
					'chk'	=> 2,
					'notag'	=> false);

$array['app']['pickup'] = array();
$array['app']['pickup']['displayname'] = 'Pickup';
$array['app']['pickup']['arg'] = array();
$array['app']['pickup']['arg'][0] = array('minlen'	=> 1,
					  'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN.
							   '(?:(?:%'.
							   XIVO_SRE_IPBX_AST_REGEXP_CONTEXT.
							   '|%?(?:[a-zA-Z0-9_-]*'.
							   XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
							   '[a-zA-Z0-9_-]*)+)?)?'.
							   '(?:'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_ATCONTEXT.')?$/');

$array['app']['playback'] = array();
$array['app']['playback']['displayname'] = 'Playback';
$array['app']['playback']['arg'] = array();
$array['app']['playback']['arg'][0] = array('minlen'	=> 1,
					    'notag'	=> false,
					    'cast'	=> 'sgraph',
					    'regexp'	=> '/^[^\|,]+$/');
$array['app']['playback']['arg'][1] = array('set'	=> false,
					    'chk'	=> 2,
					    'ustr'	=> array('skip','noanswer','j'));

$array['app']['read'] = array();
$array['app']['read']['displayname'] = 'Read';
$array['app']['read']['arg'] = array();
$array['app']['read']['arg'][0] = array('minlen'	=> 1,
					'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_CONTEXT.'$/');
$array['app']['read']['arg'][1] = array('set'		=> false,
					'chk'		=> 2,
					'notag'		=> false,
					'cast'		=> 'sgraph',
					'regexp'	=> '/^[^\|,]+$/');
$array['app']['read']['arg'][2] = array('set'		=> false,
					'chk'		=> 2,
					'cast'		=> 'uint',
					'between'	=> array(0,255));
$array['app']['read']['arg'][3] = array('set'		=> false,
					'chk'		=> 2,
					'uchar'		=> array('s','i','n'));
$array['app']['read']['arg'][4] = array('set'		=> false,
					'chk'		=> 2,
					'cast'		=> 'uint');
$array['app']['read']['arg'][5] = array('set'		=> false,
					'chk'		=> 2,
					'cast'		=> 'uint');

$array['app']['record'] = array();
$array['app']['record']['displayname'] = 'Record';
$array['app']['record']['appdata'] = 'recordings/%s|%s|%s|%s';
$array['app']['record']['arg'] = array();
$array['app']['record']['arg'][0] = array('minlen'	=> 4,
					  'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_AUDIOFILE.'$/');
$array['app']['record']['arg'][1] = array('set'		=> false,
					  'chk'		=> 2,
					  'cast'	=> 'ufloat',
					  'between'	=> array(0,30));
$array['app']['record']['arg'][2] = array('set'		=> false,
					  'chk'		=> 2,
					  'cast'	=> 'ufloat',
					  'between'	=> array(0,600));
$array['app']['record']['arg'][3] = array('set'		=> false,
					  'chk'		=> 2,
					  'uchar'	=> array('a','n','q','s','t'));

$array['app']['responsetimeout'] = array();
$array['app']['responsetimeout']['displayname'] = 'ResponseTimeout';
$array['app']['responsetimeout']['app'] = 'Set';
$array['app']['responsetimeout']['appdata'] = 'TIMEOUT(response)=%s';
$array['app']['responsetimeout']['arg'] = array();
$array['app']['responsetimeout']['arg'][0] = array('cast'	=> 'ufloat',
						   'between'	=> array(0,600));

$array['app']['set'] = array();
$array['app']['set']['displayname'] = 'Set';
$array['app']['set']['arg'] = array();
$array['app']['set']['arg'][0] = array('minlen'	=> 2,
				       'regexp'	=> '/^(?![_]{0,2}(?:XIVO|REAL)_)[a-zA-Z0-9_\(\)]+=[^\|,]*(?:\|[a-zA-Z0-9_\(\)]+=[^\|,]*)*$/');
$array['app']['set']['arg'][1] = array('set'	=> false,
				       'chk'	=> 2,
				       'key'	=> array('g'));

$array['app']['setcallerid'] = array();
$array['app']['setcallerid']['displayname'] = 'SetCallerID';
$array['app']['setcallerid']['app'] = 'Set';
$array['app']['setcallerid']['appdata'] = 'CALLERID(all)=%s';
$array['app']['setcallerid']['arg'] = array();
$array['app']['setcallerid']['arg'][0] = array('minlen'		=> 1,
					       'notag'		=> false,
					       'cast'		=> 'sgraph',
					       'regexp'		=> '/^[^\|,]*<[^\|,]+>$/');

$array['app']['setcidname'] = array();
$array['app']['setcidname']['displayname'] = 'SetCIDName';
$array['app']['setcidname']['app'] = 'Set';
$array['app']['setcidname']['appdata'] = 'CALLERID(name)=%s';
$array['app']['setcidname']['arg'] = array();
$array['app']['setcidname']['arg'][0] = array('set'	=> false,
					      'chk'	=> 2,
					      'notag'	=> false,
					      'cast'	=> 'sgraph',
					      'regexp'	=> '/^[^\|,]*$/');

$array['app']['setcidnum'] = array();
$array['app']['setcidnum']['displayname'] = 'SetCIDNum';
$array['app']['setcidnum']['app'] = 'Set';
$array['app']['setcidnum']['appdata'] = 'CALLERID(num)=%s';
$array['app']['setcidnum']['arg'] = array();
$array['app']['setcidnum']['arg'][0] = array('minlen'	=> 1,
					     'notag'	=> false,
					     'cast'	=> 'sgraph',
					     'regexp'	=> '/^[^\|,]+$/');

$array['app']['setlanguage'] = array();
$array['app']['setlanguage']['displayname'] = 'SetLanguage';
$array['app']['setlanguage']['app'] = 'Set';
$array['app']['setlanguage']['appdata'] = 'LANGUAGE()=%s';
$array['app']['setlanguage']['arg'] = array();
$array['app']['setlanguage']['arg'][0] = array('regexp'	=> '/^[a-zA-Z]{2,3}(?:[_-][a-zA-Z]{2,3})?$/');

$array['app']['stopmonitor'] = array();
$array['app']['stopmonitor']['displayname'] = 'StopMonitor';

$array['app']['system'] = array();
$array['app']['system']['displayname'] = 'System';
$array['app']['system']['arg'] = array();
$array['app']['system']['arg'][0] = array('minlen'	=> 1,
					  'notag'	=> false,
					  'cast'	=> 'sgraph');

$array['app']['vmauthenticate'] = array();
$array['app']['vmauthenticate']['displayname'] = 'VMAuthenticate';
$array['app']['vmauthenticate']['arg'] = array();
$array['app']['vmauthenticate']['arg'][0] = array('minlen'	=> 1,
						  'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN.
								   '(?:'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_ATCONTEXT.')?$/');
$array['app']['vmauthenticate']['arg'][1] = array('set'	=> false,
						  'chk'	=> 2,
						  'key'	=> array('s'));

$array['app']['voicemailmain'] = array();
$array['app']['voicemailmain']['displayname'] = 'VoiceMailMain';
$array['app']['voicemailmain']['arg'] = array();
$array['app']['voicemailmain']['arg'][0] = array('minlen'	=> 1,
						 'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN.
								   '(?:'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_ATCONTEXT.')?$/');
$array['app']['voicemailmain']['arg'][1] = array('set'		=> false,
						 'chk'		=> 2,
						 'uchar'	=> array('a','s'),
						 'strict'	=> false,
						 'regexp'	=> '/^[as]{0,2}?(?:g\([0-9]{0,2}\))?[as]{0,2}?$/');


$array['app']['wait'] = array();
$array['app']['wait']['displayname'] = 'Wait';
$array['app']['wait']['arg'] = array();
$array['app']['wait']['arg'][0] = array('cast'		=> 'ufloat',
					'between'	=> array(0,600));

$array['app']['waitexten'] = array();
$array['app']['waitexten']['displayname'] = 'WaitExten';
$array['app']['waitexten']['arg'] = array();
$array['app']['waitexten']['arg'][0] = array('set'	=> false,
					     'chk'	=> 2,
					     'cast'	=> 'ufloat',
					     'between'	=> array(0,600));
$array['app']['waitexten']['arg'][1] = array('set'	=> false,
					     'chk'	=> 2,
					     'regexp'	=> '/^m(\((?:[a-z0-9_-]{1,20}'.
							   '|(?:[a-z0-9_-]*'.
								XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
								'[a-z0-9_-]*)+)\))?$/');

$array['app']['waitforring'] = array();
$array['app']['waitforring']['displayname'] = 'WaitForRing';
$array['app']['waitforring']['arg'] = array();
$array['app']['waitforring']['arg'][0] = array('cast'		=> 'ufloat',
					       'between'	=> array(0,600));
/*
$array['app']['waitforsilence'] = array();
$array['app']['waitforsilence']['displayname'] = 'WaitForSilence';
$array['app']['waitforsilence']['arg'] = array();
$array['app']['waitforsilence']['arg'][0] = array('cast'	=> 'uint',
						  'between'	=> array(0,60000));
$array['app']['waitforsilence']['arg'][1] = array('set'		=> false,
						  'chk'		=> 2,
						  'cast'	=> 'uint',
						  'between'	=> array(0,10));
*/
$array['app']['waitmusiconhold'] = array();
$array['app']['waitmusiconhold']['displayname'] = 'WaitMusicOnHold';
$array['app']['waitmusiconhold']['arg'] = array();
$array['app']['waitmusiconhold']['arg'][0] = array('cast'	=> 'ufloat',
						   'between'	=> array(0,600));


$array['element'] = array();

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';

$array['element']['priority'] = array();
$array['element']['priority']['default'] = 1;
$array['element']['priority']['setdef'] = true;

$array['element']['app'] = array();
$array['element']['app']['default'] = '';

$array['element']['appdata'] = array();
$array['element']['appdata']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;


$array['filter'] = array();
$array['filter']['exten'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^(?:[saotTih]|_?\+?[0-9NXZ\*#\-\[\]]+[\.\!]?)$/');
$array['filter']['context'] = array('chk' => 2,'maxlen' => 39,'regexp' => '/^(?:'.XIVO_SRE_IPBX_AST_CONTEXT_FEATURES.
									      '|'.XIVO_SRE_IPBX_AST_CONTEXT_HANDYNUMBERS.
									      '|(?!(?:macro|xivo)-)[a-z0-9_-]+)$/');
$array['filter']['priority'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,255));
$array['filter']['app'] = array('minlen' => 1,'maxlen' => 128,'regexp' => '/^[a-z0-9#\*\.\/_@:-]+$/i');
$array['filter']['appdata'] = array('set' => false,'chk' => 2,'maxlen' => 128,'cast' => 'sgraph','notag' => false);
$array['filter']['name'] = array('set' => false,'chk' => 2,'maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['commented'] = array('bool' => true);

?>
