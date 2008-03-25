<?php

$array = array();

$array['element'] = array();

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('extenfeatures','featuremap','generalfeatures','generaliax','generalsip','generalvoicemail','group','handynumbers','incall','meetme','outcall','queue','user','uservoicemail');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';


$array['filter'] = array();
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(array(
				    			XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTIFICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['type'] = array('key' => array('extenfeatures','featuremap','generalfeatures','generaliax','generalsip','generalvoicemail','group','handynumbers','incall','meetme','outcall','queue','user','uservoicemail'));
$array['filter']['typeval'] = array('maxlen' => 255,'cast' => 'sgraph');

?>
