<?php

$array = array();

$array['element'] = array();

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['extenhash'] = array();
$array['element']['extenhash']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','group','queue','meetme','generaloutcall','outcall','incall','generalfeatures','extenfeatures','featuremap');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['filter'] = array();
$array['filter']['exten'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^[_]?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['extenhash'] = array('set' => false,'chk' => 2,'maxlen' => 40,'cast' => 'xdigit');
$array['filter']['context'] = array('set' => false,'chk' => 2,'nokey' => array('general','global','globals','hints','features','authentification','zonemessages'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);
$array['filter']['type'] = array('set' => false,'chk' => 2,'key' => array('user','group','queue','meetme','generaloutcall','outcall','incall','generalfeatures','extenfeatures','featuremap'));
$array['filter']['typeval'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');

?>
