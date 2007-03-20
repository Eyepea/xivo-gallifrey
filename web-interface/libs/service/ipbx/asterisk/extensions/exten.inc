<?php

$array = array();
$array['exten']['voicemail'] = array();
$array['exten']['voicemail']['context'] = 'features';
$array['exten']['voicemail']['exten'] = '_%s';
$array['exten']['voicemail']['_line'][] = array('priority' => 1,'app' => 'VoiceMailMain','appdata' => '${CALLERID(num)@default}');
$array['exten']['voicemail']['_line'][] = array('priority' => 2,'app' => 'Hangup','appdata' => '');

$array['exten']['fwdundo'] = array();
$array['exten']['fwdundo']['context'] = 'features';
$array['exten']['fwdundo']['exten'] = '_%s';
$array['exten']['fwdundo']['priority'] = 1;
$array['exten']['fwdundo']['app'] = 'Noop';
$array['exten']['fwdundo']['appdata'] = 'Not implemented undo all';

$array['exten']['fwdunc'] = array();
$array['exten']['fwdunc']['context'] = 'features';
$array['exten']['fwdunc']['exten'] = '_%s.';
$array['exten']['fwdunc']['app'] = 'Set';
$array['exten']['fwdunc']['appdata'] = '';
$array['exten']['fwdunc']['priority'] = 1;
$array['exten']['fwdunc']['_line'][] = array('exten' => '_%s','app' => 'Macro','appdata' => 'features,FWD/Unc/Status,renvoi-on,renvoi-off,1');
$array['exten']['fwdunc']['_line'][] = array('app' => 'Answer');
$array['exten']['fwdunc']['_line'][] = array('priority' => 2,'app' => 'Wait','appdata' => '0.5');
$array['exten']['fwdunc']['_line'][] = array('priority' => 3,'appdata' => 'USER=${CALLERID(num)}');
$array['exten']['fwdunc']['_line'][] = array('priority' => 4,'appdata' => 'DB(users/${USER}/FWD/Unc/Number)=${EXTEN:3}');
$array['exten']['fwdunc']['_line'][] = array('priority' => 5,'appdata' => 'DB(users/${USER}/FWD/Unc/Status)=1');
$array['exten']['fwdunc']['_line'][] = array('priority' => 6,'app' => 'Playback','appdata' => 'renvoi-on');
$array['exten']['fwdunc']['_line'][] = array('priority' => 7,'app' => 'Hangup');

$array['exten']['fwdrna'] = array();
$array['exten']['fwdrna']['context'] = 'features';
$array['exten']['fwdrna']['exten'] = '_%s.';
$array['exten']['fwdrna']['app'] = 'Set';
$array['exten']['fwdrna']['appdata'] = '';
$array['exten']['fwdrna']['priority'] = 1;
$array['exten']['fwdrna']['_line'][] = array('exten' => '_%s','app' => 'Macro','appdata' => 'features,FWD/RNA/Status,renvoi-on,renvoi-off,1');
$array['exten']['fwdrna']['_line'][] = array('app' => 'Answer');
$array['exten']['fwdrna']['_line'][] = array('priority' => 2,'app' => 'Wait','appdata' => '0.5');
$array['exten']['fwdrna']['_line'][] = array('priority' => 3,'appdata' => 'USER=${CALLERID(num)}');
$array['exten']['fwdrna']['_line'][] = array('priority' => 4,'appdata' => 'DB(users/${USER}/FWD/RNA/Number)=${EXTEN:3}');
$array['exten']['fwdrna']['_line'][] = array('priority' => 5,'appdata' => 'DB(users/${USER}/FWD/RNA/Status)=1');
$array['exten']['fwdrna']['_line'][] = array('priority' => 6,'app' => 'Playback','appdata' => 'renvoi-on');
$array['exten']['fwdrna']['_line'][] = array('priority' => 7,'app' => 'Hangup');

$array['exten']['fwdbusy'] = array();
$array['exten']['fwdbusy']['context'] = 'features';
$array['exten']['fwdbusy']['exten'] = '_%s.';
$array['exten']['fwdbusy']['app'] = 'Set';
$array['exten']['fwdbusy']['appdata'] = '';
$array['exten']['fwdbusy']['priority'] = 1;
$array['exten']['fwdbusy']['_line'][] = array('exten' => '_%s','app' => 'Macro','appdata' => 'features,FWD/Busy/Status,renvoi-on,renvoi-off,1');
$array['exten']['fwdbusy']['_line'][] = array('app' => 'Answer');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 2,'app' => 'Wait','appdata' => '0.5');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 3,'appdata' => 'USER=${CALLERID(num)}');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 4,'appdata' => 'DB(users/${USER}/FWD/Busy/Number)=${EXTEN:3}');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 5,'appdata' => 'DB(users/${USER}/FWD/Busy/Status)=1');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 6,'app' => 'Playback','appdata' => 'renvoi-on');
$array['exten']['fwdbusy']['_line'][] = array('priority' => 7,'app' => 'Hangup');

$array['exten']['record'] = array();
$array['exten']['record']['context'] = 'features';
$array['exten']['record']['exten'] = '_%s.';
$array['exten']['record']['app'] = 'Set';
$array['exten']['record']['appdata'] = '1';
$array['exten']['record']['_line'][] = array('priority' => 1,'app' => 'Wait');
$array['exten']['record']['_line'][] = array('priority' => 2,'appdata' => 'ROOT=/usr/share/asterisk/sounds/webinterface/recordings');
$array['exten']['record']['_line'][] = array('priority' => 3,'appdata' => '${ROOT}/${CALLERID(num)}:wav');
$array['exten']['record']['_line'][] = array('priority' => 4,'app' => 'Wait');
$array['exten']['record']['_line'][] = array('priority' => 5,'app' => 'ControlPlayback','appdata' => '${ROOT}/${CALLERID(num)},4000,1,2,3,4,5');
$array['exten']['record']['_line'][] = array('priority' => 6,'app' => 'Hangup','appdata' => '');

?>
