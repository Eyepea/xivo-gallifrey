<?php

define('XIVO_PATH_ROOT',dirname(__FILE__));

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'object'.DIRECTORY_SEPARATOR.'xivo.inc');

$_XIVO = &xivo_gct::set_get(new xivo());

$_CF = &xivo_gat::get('_CF');
$_QR = &xivo_gat::get('_QR');
$_URL = &xivo_gat::load_get('url',XIVO_PATH_OBJECTCONF);

$_I18N = &xivo_gct::get('xivo_i18n');
$_I18N->load_file('xivo');

$_QRY = &xivo_gct::get('xivo_query');
$_SRE = &xivo_gct::get('xivo_service');
$_USR = &xivo_gct::get('_USR');

if(defined('XIVO_TPL_SPACE') === false)
	$tpl_space = 'www';
else
	$tpl_space = XIVO_TPL_SPACE;

if(isset($_CF['tpl'][$tpl_space]) === false)
	$tpl_space = false;

switch($tpl_space)
{
	case 'www':
		xivo::load_class('xivo_tpl');
		$_HTML = &new xivo_tpl($_CF['tpl']['www'],array('menu','url','dhtml'),$_URL);
		break;
	case 'json':
		xivo::load_class('xivo_tpl');
		$_HTML = &new xivo_tpl($_CF['tpl']['json'],array('json','url','dhtml'),$_URL);
		break;	
	default:
		xivo_die('Invalid TPL SPACE');
}

if(($prepend = $_HTML->get_prepend()) !== false)
	include($prepend);

?>
