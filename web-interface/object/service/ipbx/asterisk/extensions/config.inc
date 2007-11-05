<?php

$array = array();

$array['app'] = array();
$array['app']['macro'] = array('name' => 'Macro');
$array['app']['answer'] = array('name' => 'Answer');
$array['app']['hangup'] = array('name' => 'Hangup');
$array['app']['pickup'] = array('name' => 'Pickup');
$array['app']['playback'] = array('name' => 'Playback');
$array['app']['set'] = array('name' => 'Set');
$array['app']['wait'] = array('name' => 'Wait');
$array['app']['record'] = array('name' => 'Record');
$array['app']['voicemailmain'] = array('name' => 'VoiceMailMain');
$array['app']['agentlogin'] = array('name' => 'AgentLogin');
$array['app']['agentcallbacklogin'] = array('name' => 'AgentCallBackLogin');
$array['app']['system'] = array('name' => 'System');
$array['app']['chanspy'] = array('name' => 'ChanSpy');
$array['app']['vmauthenticate'] = array('name' => 'VMauthenticate');
$array['app']['directory'] = array('name' => 'Directory');
$array['app']['goto'] = array('name' => 'GoTo');

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
$array['filter']['exten'] = array('minlen' => 1,'maxlen' => 40,'cast' => 'graph');
$array['filter']['context'] = array('chk' => 2,'maxlen' => 39,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['priority'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,255));
$array['filter']['app'] = array('minlen' => 1,'maxlen' => 128,'regexp' => '/^[a-z0-9#\*\.\/_@:-]+$/i');
$array['filter']['appdata'] = array('set' => false,'chk' => 2,'minlen' => 0,'maxlen' => 128,'cast' => 'sgraph','notag' => false);
$array['filter']['name'] = array('set' => false,'chk' => 2,'maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['commented'] = array('bool' => true);

?>
