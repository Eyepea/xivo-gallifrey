<?php

$array = array();

$array['var_metric'] = array();
$array['var_metric']['range'] = 1000000;

$array['element'] = array();

$array['element']['agent'] = array();
$array['element']['agent']['default'] = '';

$array['element']['ackcall'] = array();
$array['element']['ackcall']['value'] = array('no','yes','always');
$array['element']['ackcall']['default'] = 'no';

$array['element']['autologoff'] = array();
$array['element']['autologoff']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['autologoff']['default'] = 0;
$array['element']['autologoff']['setdef'] = true;

$array['element']['wrapuptime'] = array();
$array['element']['wrapuptime']['value'] = array(0,
						 5000 => 5,10000 => 10,15000 => 15,
						 20000 => 20,25000 => 25,30000 => 30,
						 35000 => 35,40000 => 40,45000 => 45,
						 50000 => 50,55000 => 55,60000 => 60);
$array['element']['wrapuptime']['default'] = 0;
$array['element']['wrapuptime']['setdef'] = true;

$array['element']['maxlogintries'] = array();
$array['element']['maxlogintries']['value'] = array(0,1,2,3,4,5,6,7,8,9,10);
$array['element']['maxlogintries']['default'] = 3;
$array['element']['maxlogintries']['setdef'] = true;

$array['element']['goodbye'] = array();
$array['element']['goodbye']['default'] = 'vm-goodbye';
$array['element']['goodbye']['setdef'] = true;

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = 'default';

$array['element']['updatecdr'] = array();
$array['element']['updatecdr']['value'] = array('no','yes');
$array['element']['updatecdr']['default'] = 'no';
$array['element']['updatecdr']['set'] = true;

$array['element']['recordagentcalls'] = array();
$array['element']['recordagentcalls']['value'] = array('no','yes');
$array['element']['recordagentcalls']['default'] = 'no';
$array['element']['recordagentcalls']['set'] = true;

$array['element']['createlink'] = array();
$array['element']['createlink']['value'] = array('no','yes');
$array['element']['createlink']['default'] = 'no';
$array['element']['createlink']['set'] = true;

$array['element']['recordformat'] = array();
$array['element']['recordformat']['value'] = array(
						XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
						XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
						XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM);
$array['element']['recordformat']['default'] = XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV;
$array['element']['recordformat']['setdef'] = true;

$array['element']['urlprefix'] = array();
$array['element']['urlprefix']['default'] = '';

$array['element']['custom_beep'] = array();
$array['element']['custom_beep']['default'] = 'beep';
$array['element']['custom_beep']['setdef'] = true;

$array['filter'] = array();
$array['filter']['agent'] = array('preg' => '/^[0-9\*#]+(,[0-9\*#]*(,[^,\a\t\r\n\f\0\x0B\x1B]+)?)?$/','minlen' => 1,'maxlen' => 128);
$array['filter']['group'] = array('cast' => 'uint','between' => array(1,63));
$array['filter']['ackcall'] = array('arrkey' => array('no','yes','always'));
$array['filter']['autologoff'] = array('set' => false,'chk' => 2,'arrkey' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['wrapuptime'] = array('set' => false,'chk' => 2,'arrkey' => array(0,5000,10000,15000,20000,25000,30000,35000,40000,45000,50000,55000,60000));
$array['filter']['maxlogintries'] = array('set' => false,'chk' => 2,'arrkey' => array(0,1,2,3,4,5,6,7,8,9,10));
$array['filter']['goodbye'] = array('set' => false,'chk' => 2,'maxlen' => 128);
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['updatecdr'] = array('bool' => true);
$array['filter']['recordagentcalls'] = array('bool' => true);
$array['filter']['createlink'] = array('bool' => true);
$array['filter']['recordformat'] = array('arrkey' => array(
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49,
					XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM));
$array['filter']['urlprefix'] = array('set' => false,'chk' => 2,'minlen' => 3,'maxlen' => 128);
$array['filter']['custom_beep'] = array('set' => false,'chk' => 2,'maxlen' => 50);

?>
