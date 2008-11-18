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
$array['element']['type']['value'] = array('extenfeatures',
					   'featuremap',
					   'generalfeatures',
					   'group',
					   'handynumbers',
					   'incall',
					   'meetme',
					   'outcall',
					   'queue',
					   'user',
					   'voicemenu');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['filter'] = array();
$array['filter']['exten'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^_?\+?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['extenhash'] = array('set' => false,'chk' => 2,'maxlen' => 40,'cast' => 'xdigit');
$array['filter']['context'] = array('set' => false,'chk' => 2,'maxlen' => 39,'regexp' => '/^'.XIVO_SRE_IPBX_AST_REGEXP_CONTEXT.'$/');
$array['filter']['type'] = array('key' => array('extenfeatures',
						'featuremap',
						'generalfeatures',
						'group',
						'handynumbers',
						'incall',
						'meetme',
						'outcall',
						'queue',
						'user',
						'voicemenu'));
$array['filter']['typeval'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');

?>
