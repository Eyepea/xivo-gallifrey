<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['interface'] = array();
$array['element']['interface']['default'] = '';

$array['element']['intfsuffix'] = array();
$array['element']['intfsuffix']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['element']['protocol'] = array();
$array['element']['protocol']['default'] = XIVO_SRE_IPBX_AST_PROTO_CUSTOM;
$array['element']['protocol']['forcedef'] = true;

$array['element']['category'] = array();
$array['element']['category']['default'] = 'trunk';
$array['element']['category']['forcedef'] = true;


$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_-]+$/i','minlen' => 1,'maxlen' => 40);
$array['filter']['context'] = array('set' => false,
				    'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(array(
				    			XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTIFICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['interface'] = array('regexp' => '/^[a-z0-9#\*\.\/_@:\-\$\{\}]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['intfsuffix'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9]+$/i','maxlen' => 32);
$array['filter']['commented'] = array('bool' => true);
$array['filter']['protocol'] = array('set' => false);
$array['filter']['category'] = array('set' => false);

?>
