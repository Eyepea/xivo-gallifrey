<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['displayname'] = array();
$array['element']['displayname']['default'] = '';

$array['element']['entity'] = array();
$array['element']['entity']['default'] = '';
$array['element']['entity']['null'] = true;

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] = array('callback' => 'xivo_service_asterisk::chk_context',
				 'callbackarg' => array(array(
				    			XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['displayname'] = array('cast' => 'sgraph','minlen' => 3,'maxlen' => 128);
$array['filter']['entity'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_\.-]+$/','maxlen' => 64);
$array['filter']['commented'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
