<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['timeout'] = array();
$array['element']['timeout']['value'] = array(5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['timeout']['default'] = 30;

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = '';

$array['element']['announce'] = array();
$array['element']['announce']['default'] = '';

$array['element']['strategy'] = array();
$array['element']['strategy']['value'] = array('ringall','roundrobin','leastrecent','fewestcalls','random','rrmemory');
$array['element']['strategy']['default'] = 'ringall';

$array['element']['servicelevel'] = array();
$array['element']['servicelevel']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['retry'] = array();
$array['element']['retry']['value'] = array(5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['retry']['default'] = 5;

$array['element']['weight'] = array();
$array['element']['weight']['value'] = array(0,1,2,3,4,5,6,7,8,9,10);
$array['element']['weight']['default'] = 0;

$array['element']['wrapuptime'] = array();
$array['element']['wrapuptime']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['wrapuptime']['default'] = 0;

$array['element']['maxlen'] = array();
$array['element']['maxlen']['default'] = '';

$array['element']['announce-frequency'] = array();
$array['element']['announce-frequency']['default'] = '';

$array['element']['periodic-announce-frequency'] = array();
$array['element']['periodic-announce-frequency']['default'] = '';

$array['element']['announce-holdtime'] = array();
$array['element']['announce-holdtime']['value'] = array('yes','no','once');
$array['element']['announce-holdtime']['default'] = 'no';

$array['element']['announce-round-seconds'] = array();
$array['element']['announce-round-seconds']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['announce-round-seconds']['default'] = 0;

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

$array['element']['leavewhenempty'] = array();
$array['element']['leavewhenempty']['value'] = array('no','yes','strict');
$array['element']['leavewhenempty']['default'] = 'no';

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
$array['element']['memberdelay']['default'] = '';

$array['element']['timeoutrestart'] = array();
$array['element']['timeoutrestart']['value'] = array('no','yes');
$array['element']['timeoutrestart']['default'] = 'no';
$array['element']['timeoutrestart']['set'] = true;


$array['filter'] = array();
$array['filter']['name'] = array('preg' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['timeout'] = array('arrkey' => array(5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 128);
$array['filter']['announce'] = array('set' => false,'chk' => 2,'maxlen' => 128);
$array['filter']['strategy'] = array('set' => false,'chk' => 2,'arrkey' => array('ringall','roundrobin','leastrecent','fewestcalls','random','rrmemory'));
$array['filter']['servicelevel'] = array('set' => false,'chk' => 2,'cast' => 'int');
$array['filter']['context'] = array('set' => false,'chk' => 2,'unarrkey' => array('general','globals'),'preg' => '/^[a-z0-9_-]+$/','maxlen' => 80);
$array['filter']['retry'] = array('set' => false,'chk' => 2,'arrkey' => array(5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['weight'] = array('set' => false,'chk' => 2,'between' => array(0,10));
$array['filter']['wrapuptime'] = array('set' => false,'chk' => 2,'arrkey' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['maxlen'] = array('set' => false,'chk' => 2,'cast' => 'uint');
$array['filter']['announce-frequency'] = array('set' => false,'chk' => 2,'cast' => 'int');
$array['filter']['periodic-announce-frequency'] = array('set' => false,'chk' => 2,'cast' => 'int');
$array['filter']['announce-holdtime'] = array('set' => false,'chk' => 2,'arrkey' => array('yes','no','once'));
$array['filter']['announce-round-seconds'] = array('set' => false,'chk' => 2,'arrkey' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['monitor-format'] = array('set' => false,'chk' => 2,'arrkey' => array('gsm','wav','wav49'));
$array['filter']['monitor-join'] = array('set' => false,'bool' => true);
$array['filter']['joinempty'] = array('set' => false,'chk' => 2,'arrkey' => array('no','yes','strict'));
$array['filter']['leavewhenempty'] = array('set' => false,'chk' => 2,'arrkey' => array('no','yes','strict'));
$array['filter']['eventwhencalled'] = array('set' => false,'bool' => true);
$array['filter']['eventmemberstatus'] = array('set' => false,'bool' => true);
$array['filter']['reportholdtime'] = array('set' => false,'bool' => true);
$array['filter']['memberdelay'] = array('set' => false,'chk' => 2,'cast' => 'int');
$array['filter']['timeoutrestart'] = array('set' => false,'bool' => true);

?>
