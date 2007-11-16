<?php

$array = array();

$array['element'] = array();

$array['element']['uniqueid'] = array();
$array['element']['uniqueid']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';
$array['element']['context']['setdef'] = true;

$array['element']['mailbox'] = array();
$array['element']['mailbox']['default'] = '';

$array['element']['password'] = array();
$array['element']['password']['default'] = '';

$array['element']['fullname'] = array();
$array['element']['fullname']['default'] = '';

$array['element']['email'] = array();
$array['element']['email']['default'] = '';

$array['element']['pager'] = array();
$array['element']['pager']['default'] = '';

$array['element']['dialout'] = array();
$array['element']['dialout']['default'] = '';
$array['element']['dialout']['null'] = true;

$array['element']['callback'] = array();
$array['element']['callback']['default'] = '';
$array['element']['callback']['null'] = true;

$array['element']['exitcontext'] = array();
$array['element']['exitcontext']['default'] = '';
$array['element']['exitcontext']['null'] = true;

$array['element']['language'] = array();
$array['element']['language']['default'] = '';

$array['element']['attach'] = array();
$array['element']['attach']['value'] = array(0,1);
$array['element']['attach']['default'] = 1;
$array['element']['attach']['set'] = true;

$array['element']['saycid'] = array();
$array['element']['saycid']['value'] = array(0,1);
$array['element']['saycid']['default'] = '';
$array['element']['saycid']['null'] = true;

$array['element']['review'] = array();
$array['element']['review']['value'] = array(0,1);
$array['element']['review']['default'] = '';
$array['element']['review']['null'] = true;

$array['element']['operator'] = array();
$array['element']['operator']['value'] = array(0,1);
$array['element']['operator']['default'] = '';
$array['element']['operator']['null'] = true;

$array['element']['envelope'] = array();
$array['element']['envelope']['value'] = array(0,1);
$array['element']['envelope']['default'] = '';
$array['element']['envelope']['null'] = true;

$array['element']['sayduration'] = array();
$array['element']['sayduration']['value'] = array(0,1);
$array['element']['sayduration']['default'] = '';
$array['element']['sayduration']['null'] = true;

$array['element']['saydurationm'] = array();
$array['element']['saydurationm']['value'] = array(1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['saydurationm']['default'] = '';
$array['element']['saydurationm']['null'] = true;

$array['element']['delete'] = array();
$array['element']['delete']['value'] = array(0,1);
$array['element']['delete']['default'] = 'no';
$array['element']['delete']['set'] = true;

$array['element']['forcename'] = array();
$array['element']['forcename']['value'] = array(0,1);
$array['element']['forcename']['default'] = '';
$array['element']['forcename']['null'] = true;

$array['element']['forcegreetings'] = array();
$array['element']['forcegreetings']['value'] = array(0,1);
$array['element']['forcegreetings']['default'] = '';
$array['element']['forcegreetings']['null'] = true;

$array['element']['hidefromdir'] = array();
$array['element']['hidefromdir']['value'] = array('no','yes');
$array['element']['hidefromdir']['default'] = 'yes';
$array['element']['hidefromdir']['set'] = true;

$array['element']['maxmsg'] = array();
$array['element']['maxmsg']['value'] = array(1,10,15,20,25,50,75,100,
					     125,150,175,200,300,400,500,600,700,800,900,
					     1000,2000,3000,4000,5000,6000,7000,8000,9000,9999);
$array['element']['maxmsg']['default'] = '';
$array['element']['maxmsg']['null'] = true;

$array['filter'] = array();
$array['filter']['uniqueid'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-]+$/i','maxlen' => 20);
$array['filter']['context'] = array('set' => false,
				    'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES));
$array['filter']['mailbox'] = array('minlen' => 1,'maxlen' => 40,'cast' => '/^[0-9#\*]+$/');
$array['filter']['password'] = array('regexp' => '/^[0-9#\*]+$/','maxlen' => 80);
$array['filter']['fullname'] = array('cast' => 'sgraph','maxlen' => 80);
$array['filter']['email'] = array('set' => false,'chk' => 2,'callback' => 'xivo_filter::chk_email','maxlen' => 80);
$array['filter']['pager'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9#\*]+$/','maxlen' => 80);
$array['filter']['dialout'] = array('set' => false,
				    'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES));
$array['filter']['callback'] = array('set' => false,
				     'chk' => 2,
				     'callback' => 'xivo_service_asterisk::chk_context',
				     'callbackparam' => array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES));
$array['filter']['exitcontext'] = array('set' => false,
				     'chk' => 2,
				     'callback' => 'xivo_service_asterisk::chk_context',
				     'callbackparam' => array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES));
$array['filter']['language'] = array('set' => false,'chk' => 2,'minlen' => 2,'maxlen' => 2,'regexp' => '/^[a-z]{2}$/');
$array['filter']['attach'] = array('bool' => true);
$array['filter']['saycid'] = array('set' => false,'chk' => 2,'key' => array(0,1));
$array['filter']['review'] = array('set' => false,'chk' => 2,'key' => array(0,1));
$array['filter']['operator'] = array('set' => false,'chk' => 2,'key' => array(0,1));
$array['filter']['envelope'] = array('set' => false,'chk' => 2,'key' => array(0,1));
$array['filter']['sayduration'] = array('set' => false,'chk' => 2,'key' => array(0,1));
$array['filter']['saydurationm'] = array('set' => false,'chk' => 2,'key' => array(1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['sendvoicemail'] = array('set' => false,'chk' => 2,'key' => array(0,1));
$array['filter']['delete'] = array('bool' => true);
$array['filter']['forcename'] = array('set' => false,'chk' => 2,'key' => array(0,1));
$array['filter']['forcegreetings'] = array('set' => false,'chk' => 2,'key' => array(0,1));
$array['filter']['hidefromdir'] = array('bool' => true);
$array['filter']['maxmsg'] = array('set' => false,'chk' => 2,
				   'key' => array(1,10,15,20,25,50,75,100,
						  125,150,175,200,300,400,500,600,700,800,900,
						  1000,2000,3000,4000,5000,6000,7000,8000,9000,9999));

?>
