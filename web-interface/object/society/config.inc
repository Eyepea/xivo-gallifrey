<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['displayname'] = array();
$array['element']['displayname']['default'] = '';

$array['element']['phonenumber'] = array();
$array['element']['phonenumber']['default'] = '';

$array['element']['faxnumber'] = array();
$array['element']['faxnumber']['default'] = '';

$array['element']['email'] = array();
$array['element']['email']['default'] = '';

$array['element']['url'] = array();
$array['element']['url']['default'] = '';

$array['element']['address1'] = array();
$array['element']['address1']['default'] = '';

$array['element']['address2'] = array();
$array['element']['address2']['default'] = '';

$array['element']['city'] = array();
$array['element']['city']['default'] = '';

$array['element']['state'] = array();
$array['element']['state']['default'] = '';

$array['element']['zipcode'] = array();
$array['element']['zipcode']['default'] = '';

$array['element']['country'] = array();
$array['element']['country']['default'] = '';

$array['element']['disable'] = array();
$array['element']['disable']['value'] = array(0,1);
$array['element']['disable']['default'] = 0;
$array['element']['disable']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/','minlen' => 1,'maxlen' => 64);
$array['filter']['displayname'] = array('cast' => 'sgraph','minlen' => 3,'maxlen' => 64);
$array['filter']['phonenumber'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['faxnumber'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['email'] = array('set' => false,'chk' => 2,'maxlen' => 255,'callback' => 'xivo_filter::chk_email');
$array['filter']['url'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 255);
$array['filter']['address1'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 30);
$array['filter']['address2'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 30);
$array['filter']['city'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['state'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['zipcode'] = array('set' => false,'chk' => 2,'maxlen' => 16,'regexp' => '/^[a-zA-Z0-9_-]+$/');
$array['filter']['country'] = array('set' => false,'chk' => 2,'minlen' => 2,'maxlen' => 2,'regexp' => '/^[A-Z]{2}$/');
$array['filter']['disable'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
