<?php

$array = array();

$array['element'] = array();

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user',
					   'group',
					   'queue',
					   'meetme',
					   'incall');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['element']['number'] = array();
$array['element']['number']['default'] = '';


$array['filter'] = array();
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(
				    			XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['type'] = array('user',
				 'group',
				 'queue',
				 'meetme',
				 'incall');
$array['filter']['typeval'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['number'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^[0-9NXZ\-\[\]]+[\.\!]?$/');

?>
