<?php

$array = array();

$array['element'] = array();

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','group','queue','meetme','incall');
$array['element']['type']['default'] = 'user';

$array['element']['numberbeg'] = array();
$array['element']['numberbeg']['default'] = '';

$array['element']['numberend'] = array();
$array['element']['numberend']['default'] = '';

$array['element']['didlength'] = array();
$array['element']['didlength']['value'] = array(1,2,3,4,5,6,7,8,9,10);
$array['element']['didlength']['default'] = 0;
$array['element']['didlength']['setdef'] = true;


$array['filter'] = array();
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(array(
				    			XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTIFICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['type'] = array('key' => array('user','group','queue','meetme','incall'));
$array['filter']['numberbeg'] = array('maxlen' => 16,'cast' => 'digit');
$array['filter']['numberend'] = array('set' => false,'chk' => 2,'maxlen' => 16,'cast' => 'digit');
$array['filter']['didlength'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,10));

?>
