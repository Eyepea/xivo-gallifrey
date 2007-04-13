<?php

$array = array();

$array['autoprov'] = array();

$array['autoprov']['prov']['url'] = '/prov';
$array['autoprov']['prov']['method'] = 'POST';
$array['autoprov']['prov']['protocol'] = 'tcp';
$array['autoprov']['prov']['application'] = 'http';

$array['autoprov']['list']['url'] = '/list';
$array['autoprov']['list']['method'] = 'GET';
$array['autoprov']['list']['protocol'] = 'tcp';
$array['autoprov']['list']['application'] = 'http';

$array['element']['modact'] = array();
$array['element']['modact']['value'] = array('guest','prov');
$array['element']['modact']['default'] = '';

$array['element']['macaddr'] = array();
$array['element']['macaddr']['default'] = '';

$array['element']['proto'] = array();
$array['element']['proto']['value'] = array('sip');

$array['element']['vendor'] = array();
$array['element']['vendor']['default'] = '';

$array['element']['model'] = array();
$array['element']['model']['default'] = '';

$array['element']['iduserfeatures'] = array();
$array['element']['iduserfeatures']['default'] = '';

$array['filter'] = array();
$array['filter']['modact'] = array('set' => false,'arrkey' => array('guest','prov'));
$array['filter']['macaddr'] = array('minlen' => 11,'maxlen' => 17,'preg' => '/^([A-F0-9]{2}:){5}([A-F0-9]{2})$/');
$array['filter']['vendor'] = array('maxlen' => 16,'preg' => '/^([0-9a-z\-_]+)$/');
$array['filter']['model'] = array('maxlen' => 16,'preg' => '/^([0-9a-z\-_]+)$/');
$array['filter']['proto'] = array('arrkey' => array('sip'));
$array['filter']['iduserfeatures'] = array('cast' => 'uint');

?>
