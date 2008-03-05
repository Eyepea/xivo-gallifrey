<?php

$array = array();
$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['ldapserverid'] = array();
$array['element']['ldapserverid']['default'] = '';

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
$array['filter']['ldapserverid'] = array('cast' => 'ulongint');
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
