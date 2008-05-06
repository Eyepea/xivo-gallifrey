<?php

$array = array();

$array['element'] = array();

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('callfilter',
					   'generalsip',
					   'generaliax',
					   'generalvoicemail',
					   'meetme',
					   'outcall',
					   'queue',
				 	   'rightcall',
					   'schedule',
					   'trunksip',
					   'trunkiax',
					   'trunkcustom',
					   'voicemail');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['element']['varname'] = array();
$array['element']['varname']['default'] = '';


$array['filter'] = array();
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(array(
				    			XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTIFICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['type'] = array('callfilter',
				 'generalsip',
				 'generaliax',
				 'generalvoicemail',
				 'meetme',
				 'outcall',
				 'queue',
				 'rightcall',
				 'schedule',
				 'trunksip',
				 'trunkiax',
				 'trunkcustom',
				 'voicemail');
$array['filter']['typeval'] = array('set' => false,'chk' => 2,'maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['varname'] = array('set' => false,'chk' => 2,'maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');

?>
