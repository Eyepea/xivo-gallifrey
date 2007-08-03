<?php

$array = array();

$array['provisioning_id'] = array();

$array['provisioning_id']['sip'] = array();
$array['provisioning_id']['sip']['prefix'] = 1;
$array['provisioning_id']['sip']['range'] = 100000;

$array['provisioning_id']['iax'] = array();
$array['provisioning_id']['iax']['prefix'] = 2;
$array['provisioning_id']['iax']['range'] = 100000;

$array['element'] = array();

$array['element']['firstname'] = array();
$array['element']['firstname']['default'] = '';

$array['element']['lastname'] = array();
$array['element']['lastname']['default'] = '';

$array['element']['ringseconds'] = array();
$array['element']['ringseconds']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['ringseconds']['default'] = 30;
$array['element']['ringseconds']['setdef'] = true;

$array['element']['simultcalls'] = array();
$array['element']['simultcalls']['value'] = array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
$array['element']['simultcalls']['default'] = 5;
$array['element']['simultcalls']['setdef'] = true;

$array['element']['popupwidget'] = array();
$array['element']['popupwidget']['value'] = array(0,1);
$array['element']['popupwidget']['default'] = 0;
$array['element']['popupwidget']['set'] = true;

$array['element']['ringgroup'] = array();
$array['element']['ringgroup']['value'] = array(0,1);
$array['element']['ringgroup']['default'] = 0;
$array['element']['ringgroup']['set'] = true;

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = '';

$array['filter'] = array();
$array['filter']['protocol'] = array('set' => true);
$array['filter']['protocolid'] = array('cast' => 'uint');
$array['filter']['firstname'] = array('chk' => 2,'maxlen' => 128);
$array['filter']['lastname'] = array('chk' => 2,'maxlen' => 128);
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 80);
$array['filter']['number'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['context'] = array('set' => false,'chk' => 2,'nokey' => array('general','globals','hints','features','authentification','zonemessages'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 80);
$array['filter']['ringseconds'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['simultcalls'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,20));
$array['filter']['popupwidget'] = array('bool' => true);
$array['filter']['ringgroup'] = array('bool' => true);
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['comment'] = array('chk' => 2,'maxlen' => 1000);

?>
