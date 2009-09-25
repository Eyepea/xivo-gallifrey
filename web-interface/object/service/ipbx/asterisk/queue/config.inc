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

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['timeout'] = array();
$array['element']['timeout']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['timeout']['default'] = 15;
$array['element']['timeout']['setdef'] = true;

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = XIVO_SRE_IPBX_AST_MOH_DEFAULT;

$array['element']['announce'] = array();
$array['element']['announce']['default'] = '';

$array['element']['strategy'] = array();
$array['element']['strategy']['value'] = array('leastrecent',
					       'fewestcalls',
					       'roundrobin',
					       'rrmemory',
					       'random',
					       'ringall');
$array['element']['strategy']['default'] = 'ringall';
$array['element']['strategy']['setdef'] = true;

$array['element']['servicelevel'] = array();
$array['element']['servicelevel']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';
$array['element']['context']['null'] = true;

$array['element']['retry'] = array();
$array['element']['retry']['value'] = array(1,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['retry']['default'] = 5;
$array['element']['retry']['setdef'] = true;

$array['element']['weight'] = array();
$array['element']['weight']['value'] = range(0,10);
$array['element']['weight']['default'] = 0;
$array['element']['weight']['setdef'] = true;

$array['element']['wrapuptime'] = array();
$array['element']['wrapuptime']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['wrapuptime']['default'] = 0;
$array['element']['wrapuptime']['setdef'] = true;

$array['element']['maxlen'] = array();
$array['element']['maxlen']['default'] = 0;
$array['element']['maxlen']['setdef'] = true;

$array['element']['announce-frequency'] = array();
$array['element']['announce-frequency']['value'] = array(0,15,30,45,60,75,90,
							 105,120,135,150,165,180,195,
							 210,225,240,255,270,285,300);
$array['element']['announce-frequency']['default'] = 0;
$array['element']['announce-frequency']['setdef'] = true;

$array['element']['periodic-announce-frequency'] = array();
$array['element']['periodic-announce-frequency']['value'] = array(0,15,30,45,60,75,90,
								  105,120,135,150,165,180,195,
								  210,225,240,255,270,285,300);
$array['element']['periodic-announce-frequency']['default'] = 0;
$array['element']['periodic-announce-frequency']['setdef'] = true;

$array['element']['announce-holdtime'] = array();
$array['element']['announce-holdtime']['value'] = array('no','yes','once');
$array['element']['announce-holdtime']['default'] = 'no';
$array['element']['announce-holdtime']['setdef'] = true;

$array['element']['announce-round-seconds'] = array();
$array['element']['announce-round-seconds']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['announce-round-seconds']['default'] = 0;
$array['element']['announce-round-seconds']['setdef'] = true;

$array['element']['monitor-format'] = array();
$array['element']['monitor-format']['value'] = array('gsm','wav','wav49');
$array['element']['monitor-format']['default'] = '';
$array['element']['monitor-format']['null'] = true;

$array['element']['monitor-type'] = array();
$array['element']['monitor-type']['value'] = array('no','mixmonitor');
$array['element']['monitor-type']['default'] = '';
$array['element']['monitor-type']['null'] = true;

$array['element']['joinempty'] = array();
$array['element']['joinempty']['value'] = array('no','yes','strict');
$array['element']['joinempty']['default'] = 'no';
$array['element']['joinempty']['setdef'] = true;

$array['element']['leavewhenempty'] = array();
$array['element']['leavewhenempty']['value'] = array('no','yes','strict');
$array['element']['leavewhenempty']['default'] = 'no';
$array['element']['leavewhenempty']['setdef'] = true;

$array['element']['ringinuse'] = array();
$array['element']['ringinuse']['value'] = array(0,1);
$array['element']['ringinuse']['default'] = 1;
$array['element']['ringinuse']['set'] = true;

$array['element']['eventwhencalled'] = array();
$array['element']['eventwhencalled']['value'] = array(0,1);
$array['element']['eventwhencalled']['default'] = 1;
$array['element']['eventwhencalled']['set'] = true;
$array['element']['eventwhencalled']['forcedef'] = true;

$array['element']['eventmemberstatus'] = array();
$array['element']['eventmemberstatus']['value'] = array(0,1);
$array['element']['eventmemberstatus']['default'] = 1;
$array['element']['eventmemberstatus']['set'] = true;
$array['element']['eventmemberstatus']['forcedef'] = true;

$array['element']['reportholdtime'] = array();
$array['element']['reportholdtime']['value'] = array(0,1);
$array['element']['reportholdtime']['default'] = 0;
$array['element']['reportholdtime']['set'] = true;

$array['element']['memberdelay'] = array();
$array['element']['memberdelay']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['memberdelay']['default'] = 0;
$array['element']['memberdelay']['setdef'] = true;

$array['element']['timeoutrestart'] = array();
$array['element']['timeoutrestart']['value'] = array(0,1);
$array['element']['timeoutrestart']['default'] = 0;
$array['element']['timeoutrestart']['set'] = true;

$array['element']['queue-youarenext'] = array();
$array['element']['queue-youarenext']['default'] = 'queue-youarenext';
$array['element']['queue-youarenext']['setdef'] = true;

$array['element']['queue-thereare'] = array();
$array['element']['queue-thereare']['default'] = 'queue-thereare';
$array['element']['queue-thereare']['setdef'] = true;

$array['element']['queue-callswaiting'] = array();
$array['element']['queue-callswaiting']['default'] = 'queue-callswaiting';
$array['element']['queue-callswaiting']['setdef'] = true;

$array['element']['queue-holdtime'] = array();
$array['element']['queue-holdtime']['default'] = 'queue-holdtime';
$array['element']['queue-holdtime']['setdef'] = true;

$array['element']['queue-minutes'] = array();
$array['element']['queue-minutes']['default'] = 'queue-minutes';
$array['element']['queue-minutes']['setdef'] = true;

$array['element']['queue-seconds'] = array();
$array['element']['queue-seconds']['default'] = 'queue-seconds';
$array['element']['queue-seconds']['setdef'] = true;

$array['element']['queue-thankyou'] = array();
$array['element']['queue-thankyou']['default'] = 'queue-thankyou';
$array['element']['queue-thankyou']['setdef'] = true;

$array['element']['queue-lessthan'] = array();
$array['element']['queue-lessthan']['default'] = 'queue-less-than';
$array['element']['queue-lessthan']['setdef'] = true;

$array['element']['queue-reporthold'] = array();
$array['element']['queue-reporthold']['default'] = 'queue-reporthold';
$array['element']['queue-reporthold']['setdef'] = true;

$array['element']['periodic-announce'] = array();
$array['element']['periodic-announce']['default'] = 'queue-periodic-announce';
$array['element']['periodic-announce']['setdef'] = true;

$array['element']['category'] = array();
$array['element']['category']['value'] = array('queue','group');

$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['timeout'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['announce'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['strategy'] = array('set'	=> false,
				     'chk'	=> 2,
				     'key'	=> array('leastrecent',
							 'fewestcalls',
							 'roundrobin',
							 'rrmemory',
							 'random',
							 'ringall'));
$array['filter']['servicelevel'] = array('set' => false,'chk' => 2,'cast' => 'int');
$array['filter']['context'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['retry'] = array('set' => false,'chk' => 2,'key' => array(1,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['weight'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,10));
$array['filter']['wrapuptime'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['maxlen'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,65535));
$array['filter']['announce-frequency'] = array('set'	=> false,
					       'chk'	=> 2,
					       'key' => array(0,15,30,45,60,75,90,
							      105,120,135,150,165,180,
							      210,225,240,255,270,285,300));
$array['filter']['periodic-announce-frequency'] = array('set'	=> false,
							'chk'	=> 2,
							'key' => array(0,15,30,45,60,75,90,
								       105,120,135,150,165,180,
								       210,225,240,255,270,285,300));
$array['filter']['announce-holdtime'] = array('set' => false,'chk' => 2,'key' => array('no','yes','once'));
$array['filter']['announce-round-seconds'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['monitor-format'] = array('set' => false,'chk' => 2,'key' => array('gsm','wav','wav49'));
$array['filter']['monitor-type'] = array('set' => false,'chk' => 2,'key' => array('no','mixmonitor'));
$array['filter']['joinempty'] = array('set' => false,'chk' => 2,'key' => array('no','yes','strict'));
$array['filter']['leavewhenempty'] = array('set' => false,'chk' => 2,'key' => array('no','yes','strict'));
$array['filter']['ringinuse'] = array('bool' => true);
$array['filter']['eventwhencalled'] = array('bool' => true);
$array['filter']['eventmemberstatus'] = array('bool' => true);
$array['filter']['reportholdtime'] = array('bool' => true);
$array['filter']['memberdelay'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['timeoutrestart'] = array('bool' => true);
$array['filter']['queue-youarenext'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['queue-thereare'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['queue-callswaiting'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['queue-holdtime'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['queue-minutes'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['queue-seconds'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['queue-thankyou'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['queue-lessthan'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['queue-reporthold'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['periodic-announce'] = array('set'	=> false,
					      'chk'	=> 2,
					      'regexp'	=> '/^(?:['.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.
								    preg_quote(DWHO_SEP_DIR,'/').']{1,79}\|?){1,10}$/');
$array['filter']['category'] = array('key' => array('queue','group'));

?>
