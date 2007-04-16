<?php

define('XIVO_PATH_ROOT',dirname(__FILE__));

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'object'.DIRECTORY_SEPARATOR.'xivo.inc');

$_XIVO = &xivo_gct::set_get(new xivo());

$_CF = &xivo_gat::get('_CF');
$_QR = &xivo_gat::get('_QR');
$_PI = &xivo_gat::get('_PI');

$_QRY = &xivo_gct::get('xivo_query');
$_SRE = &xivo_gct::get('xivo_service');
$_USR = &xivo_gct::get('_USR');

xivo::load_class('xivo_tpl');
$_HTML = &new xivo_tpl($_CF['tpl']['www'],array('menu','url','dhtml'));

if(($prepend = $_HTML->get_prepend()) !== false)
	include($prepend);

?>
