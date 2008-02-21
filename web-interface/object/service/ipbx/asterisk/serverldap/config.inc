<?php

$array = array();
$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['host'] = array();
$array['element']['host']['default'] = '';

$array['element']['port'] = array();
$array['element']['port']['default'] = 389;

$array['element']['ssl'] = array();
$array['element']['ssl']['value'] = array(0,1);
$array['element']['ssl']['default'] = 0;
$array['element']['ssl']['set'] = true;

$array['element']['protocolversion'] = array();
$array['element']['protocolversion']['value'] = array('2','3');
$array['element']['protocolversion']['default'] = '3';
$array['element']['protocolversion']['setdef'] = true;

$array['element']['user'] = array();
$array['element']['user']['default'] = '';

$array['element']['passwd'] = array();
$array['element']['passwd']['default'] = '';

$array['element']['basedn'] = array();
$array['element']['basedn']['default'] = '';

$array['element']['filter'] = array();
$array['element']['filter']['default'] = '';

$array['element']['attrdisplayname'] = array();
$array['element']['attrdisplayname']['default'] = '';

$array['element']['attrphonenumber'] = array();
$array['element']['attrphonenumber']['default'] = '';

$array['element']['additionaltype'] = array();
$array['element']['additionaltype']['value'] = array('office','home','mobile','fax','other','custom');
$array['element']['additionaltype']['default'] = 'office';

$array['element']['additionaltext'] = array();
$array['element']['additionaltext']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['host'] = array('callback' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['port'] = array('cast' => 'uint','between' => array(1,65535));
$array['filter']['ssl'] = array('bool' => true);
$array['filter']['protocolversion'] = array('key' => array('2','3'));
$array['filter']['user'] = array('maxlen' => 255,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/');
$array['filter']['passwd'] = array('maxlen' => 255,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['filter']['basedn'] = array('minlen' => 1,'maxlen' => 255,'callback' => 'ldap_explode_dn','callbackparam' => 1);
$array['filter']['filter'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph','notag' => false);
$array['filter']['attrdisplayname'] = array('minlen' => 1,'maxlen' => 255,'regexp' => '/^(?:(?:[a-zA-Z0-9-]+|[0-9]+(?:\.[0-9]+)*)[,]?)+$/');
$array['filter']['attrphonenumber'] = array('minlen' => 1,'maxlen' => 255,'regexp' => '/^(?:(?:[a-zA-Z0-9-]+|[0-9]+(?:\.[0-9]+)*)[,]?)+$/');
$array['filter']['additionaltype'] = array('key' => array('office','home','mobile','fax','other','custom'));
$array['filter']['additionaltext'] = array('set' => false,'chk' => 2,'maxlen' => 16,'cast' => 'sgraph');
$array['filter']['commented'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
