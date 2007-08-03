<?php

$array['element'] = array();

$array['element']['queue_name'] = array();
$array['element']['queue_name']['default'] = '';

$array['element']['interface'] = array();
$array['element']['interface']['default'] = '';

$array['element']['penalty'] = array();
$array['element']['penalty']['default'] = 0;
$array['element']['penalty']['value'] = array(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
$array['element']['penalty']['setdef'] = true;

$array['element']['call-limit'] = array();
$array['element']['call-limit']['default'] = 0;
$array['element']['call-limit']['value'] = array(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
$array['element']['call-limit']['setdef'] = true;

$array['element']['chantype'] = array();
$array['element']['chantype']['default'] = 'default';
$array['element']['chantype']['value'] = array('default',XIVO_SRE_IPBX_AST_CHAN_LOCAL);
$array['element']['chantype']['unset'] = true;

$array['element']['usertype'] = array();
$array['element']['usertype']['value'] = array('user','agent');

$array['element']['userid'] = array();
$array['element']['userid']['default'] = 0;

$array['element']['channel'] = array();
$array['element']['channel']['value'] = array(
						XIVO_SRE_IPBX_AST_CHAN_SIP,
						XIVO_SRE_IPBX_AST_CHAN_IAX,
						XIVO_SRE_IPBX_AST_CHAN_LOCAL,
						XIVO_SRE_IPBX_AST_CHAN_AGENT);

$array['element']['category'] = array();
$array['element']['category']['value'] = array('queue','group');

$array['filter'] = array();
$array['filter']['queue_name'] = array('regexp' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['interface'] = array('regexp' => '/^[a-z0-9#\*\.\/_@:-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['penalty'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,20));
$array['filter']['call-limit'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,20));
$array['filter']['usertype'] = array('key' => array('user','agent'));
$array['filter']['userid'] = array('cast' => 'uint');
$array['filter']['channel'] = array('key' => array(
							XIVO_SRE_IPBX_AST_CHAN_SIP,
							XIVO_SRE_IPBX_AST_CHAN_IAX,
							XIVO_SRE_IPBX_AST_CHAN_LOCAL,
							XIVO_SRE_IPBX_AST_CHAN_AGENT));
$array['filter']['category'] = array('key' => array('queue','group'));

?>
