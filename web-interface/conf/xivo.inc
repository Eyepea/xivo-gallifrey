<?php

define('XIVO_PATH_LIBS',XIVO_PATH_ROOT.'/libs');
define('XIVO_PATH_CONF',XIVO_PATH_ROOT.'/conf');
define('XIVO_PATH_INIT',XIVO_PATH_ROOT.'/init');
define('XIVO_PATH_WWW',XIVO_PATH_ROOT.'/www');
define('XIVO_PATH_TPL',XIVO_PATH_ROOT.'/tpl');
define('XIVO_PATH_CLI',XIVO_PATH_ROOT.'/cli');
define('XIVO_PATH_I18N',XIVO_PATH_ROOT.'/i18n');

ini_set('max_execution_time',0);
ini_set('memory_limit','16M');
ini_set('include_path',XIVO_PATH_ROOT.PATH_SEPARATOR.XIVO_PATH_LIBS.PATH_SEPARATOR.'.');
ini_set('register_globals',true);
ini_set('magic_quotes_gpc',false);
ini_set('session.use_only_cookies',true);
ini_set('track_errors',true);

define('EID_NAME','_eid');
define('EID_TIME',7200);
define('EID_PATH','/');

$_CF = array();
$_CF['session'] = array();
$_CF['session']['cookie'] = array();
$_CF['session']['cookie']['time'] = EID_TIME;
$_CF['session']['cookie']['path'] = EID_PATH;
$_CF['session']['name'] = EID_NAME;

$_CF['part'] = array();
$_CF['site'] = array();
$_CF['i18n'] = array();

require_once(XIVO_PATH_LIBS.'/func.inc');
require_once(XIVO_PATH_LIBS.'/trac_err.inc');

switch(php_sapi_name())
{
	case 'cli':
		define('XIVO_SAPI_MODE','cli');
		break;
	default:
		define('XIVO_SAPI_MODE','default');
}

switch(getenv('WAG_MODE'))
{
	case 'DEV':
		define('XIVO_ROOT_MAIL','adc@proformatique.com');
		define('XIVO_WAG_MODE','DEV');
		define('XIVO_TE_ERR_FILE_LOG',XIVO_PATH_ROOT.'/xivo_error_log');

		error_reporting(E_ALL | E_NOTICE);

		ini_set('display_startup_errors',true);
		ini_set('display_errors',true);
		ini_set('error_log',XIVO_TE_ERR_FILE_LOG);

		$_E = new trac_err($_CF['_libs']['trac_err'],array('report_type' => (XIVO_TE_RTYPE_LOG | XIVO_TE_RTYPE_SCREEN)));
		set_error_handler(array(&$_E,'report_e_err'));

		break;
	case 'PROD':
	default:
		define('XIVO_ROOT_MAIL','adc@proformatique.com');
		define('XIVO_WAG_MODE','PROD');
		error_reporting(E_ALL ^ E_NOTICE);
		ini_set('display_startup_errors',false);
		ini_set('display_errors',false);

		define('XIVO_TE_ERR_FILE_LOG',XIVO_PATH_ROOT.'/xivo_error_log');

		$_E = new trac_err();
		set_error_handler(array(&$_E,'report_e_err'));
}

// require_once('tpl.inc');
// require_once('mysql.inc');
// require_once('query.inc');

?>
