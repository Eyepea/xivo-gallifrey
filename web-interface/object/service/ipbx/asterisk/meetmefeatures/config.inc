<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['meetmeid'] = array();
$array['element']['meetmeid']['default'] = '';

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('talk','listen','all');
$array['element']['mode']['default'] = 'all';

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = XIVO_SRE_IPBX_AST_MOH_DEFAULT;

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['poundexit'] = array();
$array['element']['poundexit']['value'] = array(0,1);
$array['element']['poundexit']['default'] = 0;
$array['element']['poundexit']['set'] = true;

$array['element']['quiet'] = array();
$array['element']['quiet']['value'] = array(0,1);
$array['element']['quiet']['default'] = 0;
$array['element']['quiet']['set'] = true;

$array['element']['record'] = array();
$array['element']['record']['value'] = array(0,1);
$array['element']['record']['default'] = 0;
$array['element']['record']['set'] = true;

$array['element']['adminmode'] = array();
$array['element']['adminmode']['value'] = array(0,1);
$array['element']['adminmode']['default'] = 0;
$array['element']['adminmode']['set'] = true;

$array['element']['announceusercount'] = array();
$array['element']['announceusercount']['value'] = array(0,1);
$array['element']['announceusercount']['default'] = 0;
$array['element']['announceusercount']['set'] = true;

$array['element']['announcejoinleave'] = array();
$array['element']['announcejoinleave']['value'] = array(0,1);
$array['element']['announcejoinleave']['default'] = 0;
$array['element']['announcejoinleave']['set'] = true;

$array['element']['alwayspromptpin'] = array();
$array['element']['alwayspromptpin']['value'] = array(0,1);
$array['element']['alwayspromptpin']['default'] = 0;
$array['element']['alwayspromptpin']['set'] = true;

$array['element']['starmenu'] = array();
$array['element']['starmenu']['value'] = array(0,1);
$array['element']['starmenu']['default'] = 0;
$array['element']['starmenu']['set'] = true;

$array['element']['enableexitcontext'] = array();
$array['element']['enableexitcontext']['value'] = array(0,1);
$array['element']['enableexitcontext']['default'] = 0;
$array['element']['enableexitcontext']['set'] = true;

$array['element']['exitcontext'] = array();
$array['element']['exitcontext']['default'] = '';
$array['element']['exitcontext']['setdef'] = true;


$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['number'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['meetmeid'] = array('cast' => 'ulongint');
$array['filter']['mode'] = array('key' => array('talk','listen','all'));
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 20);
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['poundexit'] = array('bool' => true);
$array['filter']['quiet'] = array('bool' => true);
$array['filter']['record'] = array('bool' => true);
$array['filter']['adminmode'] = array('bool' => true);
$array['filter']['announceusercount'] = array('bool' => true);
$array['filter']['announcejoinleave'] = array('bool' => true);
$array['filter']['alwayspromptpin'] = array('bool' => true);
$array['filter']['starmenu'] = array('bool' => true);
$array['filter']['enableexitcontext'] = array('bool' => true);
$array['filter']['exitcontext'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');

?>
