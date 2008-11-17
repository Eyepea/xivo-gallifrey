<?php

$array = array();

$array['element'] = array();

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['include'] = array();
$array['element']['include']['default'] = '';

$array['filter'] = array();
$array['filter']['context'] = array('callback'		=> 'xivo_service_asterisk::chk_context',
				    'callbackarg'	=> array(array(
				    				XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
								XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));

$array['filter']['include'] = array('callback'		=> 'xivo_service_asterisk::chk_context',
				    'callbackarg'	=> array(array(
				    				XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
								XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));

?>
