<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['timeout'] = array();
$array['element']['timeout']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['timeout']['default'] = 15;
$array['element']['timeout']['setdef'] = true;

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = '';

$array['element']['announce'] = array();
$array['element']['announce']['default'] = '';

$array['element']['strategy'] = array();
$array['element']['strategy']['value'] = array('ringall','roundrobin','leastrecent','fewestcalls','random','rrmemory');
$array['element']['strategy']['default'] = 'ringall';
$array['element']['strategy']['setdef'] = true;

$array['element']['servicelevel'] = array();
$array['element']['servicelevel']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['retry'] = array();
$array['element']['retry']['value'] = array(1,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['retry']['default'] = 5;
$array['element']['retry']['setdef'] = true;

$array['element']['weight'] = array();
$array['element']['weight']['value'] = array(0,1,2,3,4,5,6,7,8,9,10);
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
$array['element']['announce-frequency']['value'] = array(0 => 0,15 => 15,30 => 30,45 => 45,60 => 1,
								  75 => array(1,15),90 => array(1,30),105 => array(1,45),
								  120 => 2,135 => array(2,15),150 => array(2,30),
								  165 => array(2,45),180 => 3,195 => array(3,15),
								  210 => array(3,30),225 => array(3,45),240 => 4,
								  255 => array(4,15),270 => array(4,30),285 => array(4,45),300 => 5);
$array['element']['announce-frequency']['default'] = 0;
$array['element']['announce-frequency']['setdef'] = true;

$array['element']['periodic-announce-frequency'] = array();
$array['element']['periodic-announce-frequency']['value'] = array(0 => 0,15 => 15,30 => 30,45 => 45,60 => 1,
								  75 => array(1,15),90 => array(1,30),105 => array(1,45),
								  120 => 2,135 => array(2,15),150 => array(2,30),
								  165 => array(2,45),180 => 3,195 => array(3,15),
								  210 => array(3,30),225 => array(3,45),240 => 4,
								  255 => array(4,15),270 => array(4,30),285 => array(4,45),300 => 5);
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

$array['element']['monitor-join'] = array();
$array['element']['monitor-join']['value'] = array('no','yes');
$array['element']['monitor-join']['default'] = 'no';
$array['element']['monitor-join']['set'] = true;

$array['element']['joinempty'] = array();
$array['element']['joinempty']['value'] = array('no','yes','strict');
$array['element']['joinempty']['default'] = 'no';
$array['element']['joinempty']['setdef'] = true;

$array['element']['leavewhenempty'] = array();
$array['element']['leavewhenempty']['value'] = array('no','yes','strict');
$array['element']['leavewhenempty']['default'] = 'no';
$array['element']['leavewhenempty']['setdef'] = true;

$array['element']['eventwhencalled'] = array();
$array['element']['eventwhencalled']['value'] = array('no','yes');
$array['element']['eventwhencalled']['default'] = 'no';
$array['element']['eventwhencalled']['set'] = true;

$array['element']['eventmemberstatus'] = array();
$array['element']['eventmemberstatus']['value'] = array('no','yes');
$array['element']['eventmemberstatus']['default'] = 'no';
$array['element']['eventmemberstatus']['set'] = true;

$array['element']['reportholdtime'] = array();
$array['element']['reportholdtime']['value'] = array('no','yes');
$array['element']['reportholdtime']['default'] = 'no';
$array['element']['reportholdtime']['set'] = true;

$array['element']['memberdelay'] = array();
$array['element']['memberdelay']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['memberdelay']['default'] = 0;
$array['element']['memberdelay']['setdef'] = true;

$array['element']['timeoutrestart'] = array();
$array['element']['timeoutrestart']['value'] = array('no','yes');
$array['element']['timeoutrestart']['default'] = 'no';
$array['element']['timeoutrestart']['set'] = true;

$array['element']['category'] = array();
$array['element']['category']['value'] = array('queue','group');

$array['element']['queue-youarenext'] = array();
$array['element']['queue-youarenext']['default'] = '';
$array['element']['queue-youarenext']['null'] = true;

$array['element']['queue-thereare'] = array();
$array['element']['queue-thereare']['default'] = '';
$array['element']['queue-thereare']['null'] = true;

$array['element']['queue-callswaiting'] = array();
$array['element']['queue-callswaiting']['default'] = '';
$array['element']['queue-callswaiting']['null'] = true;

$array['element']['queue-holdtime'] = array();
$array['element']['queue-holdtime']['default'] = '';
$array['element']['queue-holdtime']['null'] = true;

$array['element']['queue-minutes'] = array();
$array['element']['queue-minutes']['default'] = '';
$array['element']['queue-minutes']['null'] = true;

$array['element']['queue-seconds'] = array();
$array['element']['queue-seconds']['default'] = '';
$array['element']['queue-seconds']['null'] = true;

$array['element']['queue-thankyou'] = array();
$array['element']['queue-thankyou']['default'] = '';
$array['element']['queue-thankyou']['null'] = true;

$array['element']['queue-lessthan'] = array();
$array['element']['queue-lessthan']['default'] = '';
$array['element']['queue-lessthan']['null'] = true;

$array['element']['queue-reporthold'] = array();
$array['element']['queue-reporthold']['default'] = '';
$array['element']['queue-reporthold']['null'] = true;

$array['element']['periodic-announce'] = array();
$array['element']['periodic-announce']['default'] = '';
$array['element']['periodic-announce']['null'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['timeout'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['announce'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['strategy'] = array('set' => false,'chk' => 2,
				     'key' => array('ringall','roundrobin','leastrecent','fewestcalls','random','rrmemory'));
$array['filter']['servicelevel'] = array('set' => false,'chk' => 2,'cast' => 'int');
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['retry'] = array('set' => false,'chk' => 2,'key' => array(1,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['weight'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,10));
$array['filter']['wrapuptime'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['maxlen'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,65535));
$array['filter']['announce-frequency'] = array('set' => false,'chk' => 2,'key' => array(0,15,30,45,60,75,90,105,120,135,
											   150,165,180,210,225,240,255,270,
											   285,300));
$array['filter']['periodic-announce-frequency'] = array('set' => false,'chk' => 2,'key' => array(0,15,30,45,60,75,90,105,120,135,
											   150,165,180,210,225,240,255,270,
											   285,300));
$array['filter']['announce-holdtime'] = array('set' => false,'chk' => 2,'key' => array('no','yes','once'));
$array['filter']['announce-round-seconds'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['monitor-format'] = array('set' => false,'chk' => 2,'key' => array('gsm','wav','wav49'));
$array['filter']['monitor-join'] = array('bool' => true);
$array['filter']['joinempty'] = array('set' => false,'chk' => 2,'key' => array('no','yes','strict'));
$array['filter']['leavewhenempty'] = array('set' => false,'chk' => 2,'key' => array('no','yes','strict'));
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
$array['filter']['periodic-announce'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['category'] = array('key' => array('queue','group'));

?>
