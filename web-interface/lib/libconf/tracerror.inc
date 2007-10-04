<?php

define('XIVO_TE_ERR_TRAC',0);
define('XIVO_TE_ERR_ERROR',(E_ERROR | E_CORE_ERROR | E_COMPILE_ERROR | E_USER_ERROR));
define('XIVO_TE_ERR_WARNING',(E_WARNING | E_CORE_WARNING | E_COMPILE_WARNING | E_USER_WARNING));
define('XIVO_TE_ERR_NOTICE',(E_NOTICE | E_USER_NOTICE));
define('XIVO_TE_ERR_PARSE',E_PARSE);
define('XIVO_TE_ERR_ALL',(XIVO_TE_ERR_ERROR | XIVO_TE_ERR_WARNING | XIVO_TE_ERR_NOTICE | XIVO_TE_ERR_PARSE));

define('XIVO_TE_RTYPE_NONE',0);
define('XIVO_TE_RTYPE_LOG',1);
define('XIVO_TE_RTYPE_SCREEN',2);
define('XIVO_TE_RTYPE_MAIL',4);
define('XIVO_TE_RTYPE_ALL',7);

define('XIVO_TE_RMODE_REALTIME',1);
define('XIVO_TE_RMODE_DELAYED',2);

define('XIVO_TE_RFUNC_PRINTR',1);
define('XIVO_TE_RFUNC_VARDUMP',2);

define('XIVO_TE_CONTEXT','Global');

$array = array();

$array['level'] = array(
		XIVO_TE_ERR_TRAC	=> array('label' => 'XIVO Trace','color' => 'black'),
		XIVO_TE_ERR_ERROR	=> array('label' => 'XIVO Error','color' => 'red'),
		XIVO_TE_ERR_WARNING	=> array('label' => 'XIVO Warning','color' => 'purple'),
		XIVO_TE_ERR_NOTICE	=> array('label' => 'XIVO Notice','color' => 'blue'),
		XIVO_TE_ERR_PARSE	=> array('label' => 'XIVO Parse','color' => 'grey'));

$array['rtype'] = array(
		XIVO_TE_RTYPE_NONE	=> 'XIVO_TE_RTYPE_NONE',
		XIVO_TE_RTYPE_LOG	=> 'XIVO_TE_RTYPE_LOG',
		XIVO_TE_RTYPE_SCREEN	=> 'XIVO_TE_RTYPE_SCREEN',
		XIVO_TE_RTYPE_MAIL	=> 'XIVO_TE_RTYPE_MAIL');

$array['rmode'] = array(
		XIVO_TE_RMODE_REALTIME	=> 'XIVO_TE_RMODE_REALTIME',
		XIVO_TE_RMODE_DELAYED	=> 'XIVO_TE_RMODE_DELAYED');

$array['rfunc'] = array(
		XIVO_TE_RFUNC_PRINTR	=> 'XIVO_TE_RFUNC_PRINTR',
		XIVO_TE_RFUNC_VARDUMP	=> 'XIVO_TE_RFUNC_VARDUMP');

$array['e_context'] = array(
		E_ERROR			=> 'E_ERROR',
		E_CORE_ERROR		=> 'E_CORE_ERROR',
		E_COMPILE_ERROR		=> 'E_COMPILE_ERROR',
		E_USER_ERROR		=> 'E_USER_ERROR',
		E_WARNING		=> 'E_WARNING',
		E_CORE_WARNING		=> 'E_CORE_WARNING',
		E_COMPILE_WARNING	=> 'E_COMPILE_ERROR',
		E_USER_WARNING		=> 'E_USER_WARNING',
		E_NOTICE		=> 'E_NOTICE',
		E_USER_NOTICE		=> 'E_USER_NOTICE',
		E_PARSE			=> 'E_PARSE');

?>
