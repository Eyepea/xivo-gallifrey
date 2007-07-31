<?php

$array['element'] = array();

$array['element']['dbeg'] = array();
$array['element']['dbeg']['default'] = strftime('%Y-%m-%d');
$array['element']['dbeg']['unset'] = true;

$array['element']['dend'] = array();
$array['element']['dend']['default'] = '';
$array['element']['dend']['unset'] = true;

$array['element']['channel'] = array();
$array['element']['channel']['value'] = array(
					XIVO_SRE_IPBX_AST_CHAN_SIP,
					XIVO_SRE_IPBX_AST_CHAN_IAX,
					XIVO_SRE_IPBX_AST_CHAN_LOCAL,
					XIVO_SRE_IPBX_AST_CHAN_AGENT,
					XIVO_SRE_IPBX_AST_CHAN_ZAP,
					XIVO_SRE_IPBX_AST_CHAN_CAPI,
					XIVO_SRE_IPBX_AST_CHAN_MISDN,
					XIVO_SRE_IPBX_AST_CHAN_MGCP,
					'others',
					'unknown');
$array['element']['channel']['default'] = '';

$array['element']['disposition'] = array();
$array['element']['disposition']['value'] = array(
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_NOANSWER,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_FAILED,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_BUSY,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_ANSWERED,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_UNKNOWN);
$array['element']['disposition']['default'] = '';

$array['element']['src'] = array();
$array['element']['src']['default'] = '';

$array['element']['srcformat'] = array();
$array['element']['srcformat']['value'] = array('exact','begin','contain','end');
$array['element']['srcformat']['default'] = 'exact';
$array['element']['srcformat']['unset'] = true;

$array['element']['dst'] = array();
$array['element']['dst']['default'] = '';

$array['element']['dstformat'] = array();
$array['element']['dstformat']['value'] = array('exact','begin','contain','end');
$array['element']['dstformat']['default'] = 'exact';
$array['element']['dstformat']['unset'] = true;

$array['element']['clid'] = array();
$array['element']['clid']['default'] = '';

$array['element']['clidformat'] = array();
$array['element']['clidformat']['value'] = array('exact','begin','contain','end');
$array['element']['clidformat']['default'] = 'exact';
$array['element']['clidformat']['unset'] = true;

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = '';

$array['element']['accountcodeformat'] = array();
$array['element']['accountcodeformat']['value'] = array('exact','begin','contain','end');
$array['element']['accountcodeformat']['default'] = 'exact';
$array['element']['accountcodeformat']['unset'] = true;

$array['element']['userfield'] = array();
$array['element']['userfield']['default'] = '';

$array['element']['userfieldformat'] = array();
$array['element']['userfieldformat']['value'] = array('exact','begin','contain','end');
$array['element']['userfieldformat']['default'] = 'exact';
$array['element']['userfieldformat']['unset'] = true;

$array['element']['dubeg'] = array();
$array['element']['dubeg']['default'] = '';

$array['element']['duend'] = array();
$array['element']['duend']['default'] = '';

$array['element']['dubeg'] = array();
$array['element']['dubeg']['default'] = '';
$array['element']['dubeg']['unset'] = true;

$array['element']['duend'] = array();
$array['element']['duend']['default'] = '';
$array['element']['duend']['unset'] = true;

$array['element']['dubegunit'] = array();
$array['element']['dubegunit']['default'] = 'second';
$array['element']['dubegunit']['value'] = array('seconds','minutes');
$array['element']['dubegunit']['unset'] = true;

$array['element']['duendunit'] = array();
$array['element']['duendunit']['default'] = 'second';
$array['element']['duendunit']['value'] = array('seconds','minutes');
$array['element']['duendunit']['unset'] = true;

$array['filter'] = array();
$array['filter']['dbeg'] = array('set' => false,'chk' => 2,'minlen' => 4,'maxlen' => 10,
				 'preg' => '/^2[0-9]{3}(-(0?[1-9]|1[0-2])(-(0?[1-9]|1[0-9]|2[0-9]|3[0-1]))?)?$/');
$array['filter']['dend'] = array('set' => false,'chk' => 2,'minlen' => 4,'maxlen' => 10,
				 'preg' => '/^2[0-9]{3}(-(0?[1-9]|1[0-2])(-(0?[1-9]|1[0-9]|2[0-9]|3[0-1]))?)?$/');
$array['filter']['channel'] = array('set' => false,'chk' => 2,'arrkey' => array(
					XIVO_SRE_IPBX_AST_CHAN_SIP,
					XIVO_SRE_IPBX_AST_CHAN_IAX,
					XIVO_SRE_IPBX_AST_CHAN_LOCAL,
					XIVO_SRE_IPBX_AST_CHAN_AGENT,
					XIVO_SRE_IPBX_AST_CHAN_ZAP,
					XIVO_SRE_IPBX_AST_CHAN_CAPI,
					XIVO_SRE_IPBX_AST_CHAN_MISDN,
					XIVO_SRE_IPBX_AST_CHAN_MGCP,
					'others',
					'unknown'));
$array['filter']['disposition'] = array('set' => false,'chk' => 2,'arrkey' => array(
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_NOANSWER,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_FAILED,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_BUSY,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_ANSWERED,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_UNKNOWN));
$array['filter']['src'] = array('set' => false,'chk' => 2,'cast' => 'cprint','maxlen' => 80);
$array['filter']['srcformat'] = array('arrkey' => array('exact','begin','contain','end'));
$array['filter']['dst'] = array('set' => false,'chk' => 2,'cast' => 'cprint','maxlen' => 80);
$array['filter']['dstformat'] = array('arrkey' => array('exact','begin','contain','end'));
$array['filter']['clid'] = array('set' => false,'chk' => 2,'cast' => 'cprint','maxlen' => 80);
$array['filter']['clidformat'] = array('arrkey' => array('exact','begin','contain','end'));
$array['filter']['accountcode'] = array('set' => false,'chk' => 2,'preg' => '/^[a-z0-9_-]+$/i','maxlen' => 20);
$array['filter']['accountcodeformat'] = array('arrkey' => array('exact','begin','contain','end'));
$array['filter']['userfield'] = array('set' => false,'chk' => 2,'preg' => '/^[a-z0-9_-]+$/i','maxlen' => 255);
$array['filter']['userfieldformat'] = array('arrkey' => array('exact','begin','contain','end'));
$array['filter']['dubeg'] = array('set' => false,'chk' => 2,'cast' => 'uint','maxlen' => 5);
$array['filter']['duend'] = array('set' => false,'chk' => 2,'cast' => 'uint','maxlen' => 5);
$array['filter']['dubegunit'] = array('arrkey' => array('seconds','minutes'));
$array['filter']['duendunit'] = array('arrkey' => array('seconds','minutes'));

?>
