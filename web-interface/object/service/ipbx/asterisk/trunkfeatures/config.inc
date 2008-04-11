<?php

$array['element'] = array();

$array['element']['protocol'] = array();
$array['element']['protocol']['value'] = array(
					XIVO_SRE_IPBX_AST_PROTO_SIP,
					XIVO_SRE_IPBX_AST_PROTO_IAX,
					XIVO_SRE_IPBX_AST_PROTO_CUSTOM);

$array['element']['protocolid'] = array();
$array['element']['protocolid']['default'] = 0;

$array['element']['registerid'] = array();
$array['element']['registerid']['default'] = 0;

$array['element']['registercommented'] = array();
$array['element']['registercommented']['value'] = array(0,1);
$array['element']['registercommented']['default'] = 0;
$array['element']['registercommented']['set'] = true;

$array['filter'] = array();
$array['filter']['protocol'] = array('key' => array(
						XIVO_SRE_IPBX_AST_PROTO_SIP,
						XIVO_SRE_IPBX_AST_PROTO_IAX,
						XIVO_SRE_IPBX_AST_PROTO_CUSTOM));
$array['filter']['protocolid'] = array('cast' => 'ulongint');
$array['filter']['registerid'] = array('set' => false,'chk' => 2,'cast' => 'ulongint');
$array['filter']['registercommented'] = array('bool' => true);

?>
