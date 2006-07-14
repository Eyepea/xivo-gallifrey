<?php

// Niveaux erreurs
define('XIVO_TE_ERR_TRAC',0);
define('XIVO_TE_ERR_ERROR',(E_ERROR | E_CORE_ERROR | E_COMPILE_ERROR | E_USER_ERROR));
define('XIVO_TE_ERR_WARNING',(E_WARNING | E_CORE_WARNING | E_COMPILE_WARNING | E_USER_WARNING));
define('XIVO_TE_ERR_NOTICE',(E_NOTICE | E_USER_NOTICE));
define('XIVO_TE_ERR_PARSE',E_PARSE);
define('XIVO_TE_ERR_ALL',(XIVO_TE_ERR_ERROR | XIVO_TE_ERR_WARNING | XIVO_TE_ERR_NOTICE | XIVO_TE_ERR_PARSE));

// Type de rapport
define('XIVO_TE_RTYPE_NONE',0);
define('XIVO_TE_RTYPE_LOG',1);
define('XIVO_TE_RTYPE_SCREEN',2);
define('XIVO_TE_RTYPE_MAIL',4);
define('XIVO_TE_RTYPE_ALL',7);

// Mode de rapport
define('XIVO_TE_RMODE_REALTIME',1);
define('XIVO_TE_RMODE_DELAYED',2);

define('XIVO_TE_CONTEXT','Global');

$_CF['_libs']['trac_err'] = array();

$_CF['_libs']['trac_err']['level'] = array(
		XIVO_TE_ERR_TRAC	=> array('label' => 'XIVO Trace','color' => 'black'),
		XIVO_TE_ERR_ERROR	=> array('label' => 'XIVO Error','color' => 'red'),
		XIVO_TE_ERR_WARNING	=> array('label' => 'XIVO Warning','color' => 'purple'),
		XIVO_TE_ERR_NOTICE	=> array('label' => 'XIVO Notice','color' => 'blue'),
		XIVO_TE_ERR_PARSE	=> array('label' => 'XIVO Parse','color' => 'grey'));

$_CF['_libs']['trac_err']['rtype'] = array(
		XIVO_TE_RTYPE_NONE	=> 'XIVO_TE_RTYPE_NONE',
		XIVO_TE_RTYPE_LOG	=> 'XIVO_TE_RTYPE_LOG',
		XIVO_TE_RTYPE_SCREEN	=> 'XIVO_TE_RTYPE_SCREEN',
		XIVO_TE_RTYPE_MAIL	=> 'XIVO_TE_RTYPE_MAIL');

$_CF['_libs']['trac_err']['rmode'] = array(
		XIVO_TE_RMODE_REALTIME	=> 'XIVO_TE_RMODE_REALTIME',
		XIVO_TE_RMODE_DELAYED	=> 'XIVO_TE_RMODE_DELAYED');

$_CF['_libs']['trac_err']['e_context'] = array(
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

class trac_err
{
	var $level = XIVO_TE_ERR_ALL;
	var $report_type = XIVO_TE_RTYPE_LOG;
	var $report_mode = XIVO_TE_RMODE_REALTIME;
	var $delayed_content = '';
	var $sapi = 'default';
	var $mail = false;
	var $conf = array();

	var $trac = array();
	var $err = array();

	function trac_err($conf,$params=array())
	{
		if(!is_array($conf))
			die('ERR : CF MUST BE AN ARRAY (error.inc)');

		$this->conf = &$conf;

		$params = (array) $params;

		if(defined('XIVO_SAPI_MODE'))
			$this->sapi = XIVO_SAPI_MODE;

		if(isset($params['level']))
			$this->level = (int) $params['level'];

		if(isset($params['report_type']))
			$this->report_type = (int) $params['report_type'];

		if(isset($params['report_mode'],$this->conf['rmode'][$params['report_mode']]))
			$this->report_mode = (int) $params['report_mode'];
		
		if(isset($params['mail']) && xivo_chk_email($params['mail']))
			$this->mail = (string) $params['mail'];
		else if(defined('XIVO_ROOT_MAIL') && xivo_chk_email(XIVO_ROOT_MAIL))
			$this->mail = (string) XIVO_ROOT_MAIL;
		else
			$this->mail = false;
	}

	function warning($msg,$context='',$report_type=0)
	{
		return($this->report_err($msg,XIVO_TE_ERR_WARNING,$context,$report_type));
	}

	function error($msg,$context='',$report_type=0)
	{
		return($this->report_err($msg,XIVO_TE_ERR_ERROR,$context,$report_type));
	}

	function notice($msg,$context='',$report_type=0)
	{
		return($this->report_err($msg,XIVO_TE_ERR_NOTICE,$context,$report_type));
	}

	function report_err($msg,$level=0,$context=XIVO_TE_CONTEXT,$report_type=null)
	{
		$level = (int) $level;

		if(($level & $this->level) === 0)
			return(false);

		$context = $context !== '' ? (string) $context : XIVO_TE_CONTEXT;

		if(!is_null($report_type) && $report_type !== XIVO_TE_RTYPE_NONE && $report_type !== $this->report_type)
		{
			return($this->display($msg,$level,$report_type,$context));
		}

		if($this->report_mode === XIVO_TE_RMODE_REALTIME)
		{
			return($this->display($msg,$level,null,$context,XIVO_TE_RMODE_REALTIME));
		}
		
		if(!isset($this->err[$level]))
			$this->err[$level] = array();

		if(!isset($this->err[$level][$context]))
			$this->err[$level][$context] = array();

		$this->err[$level][$context][] = $msg;

		return(true);
	}

	function report_e_err($errno,$errstr,$errfile,$errline)
	{
		$context = XIVO_TE_CONTEXT;
		$level = XIVO_TE_ERR_TRAC;

		$msg = '['.$errno.'] '.$errstr.' - File: '.$errfile.' - Line: '.$errline."\n";

		if(isset($this->conf['e_context'][$errno]))
		{
			$context = $this->conf['e_context'][$errno];

			if($errno & XIVO_TE_ERR_ERROR)
				$level = XIVO_TE_ERR_ERROR;
			else if($errno & XIVO_TE_ERR_WARNING)
				$level = XIVO_TE_ERR_WARNING;
			else if($errno & XIVO_TE_ERR_NOTICE)
				$level = XIVO_TE_ERR_NOTICE;
			else if($errno & XIVO_TE_ERR_PARSE)
				$level = XIVO_TE_ERR_PARSE;
		}

		$this->report_err($msg,$level,$context,$this->report_type);
	}

	function report_trac($msg,$context=XIVO_TE_CONTEXT,$report_type=null,$report_mode=0)
	{
		$context = $context !== '' ? (string) $context : XIVO_TE_CONTEXT;

		if(!is_null($report_type) && $report_type !== XIVO_TE_RTYPE_NONE && $report_type !== $this->report_type)
		{
			return($this->display($msg,0,$report_type,$context,XIVO_TE_RMODE_REALTIME));
		}

		if($report_mode === XIVO_TE_RMODE_REALTIME)
		{
			return($this->display($msg,0,null,$context,XIVO_TE_RMODE_REALTIME));
		}

		if(!isset($this->trac[$context]))
			$this->trac[$context] = array();

		$this->trac[$context][] = $msg;

		return(true);
	}

	function display($msg,$level=XIVO_TE_ERR_TRAC,$report_type=null,$context=XIVO_TE_CONTEXT,$report_mode=0)
	{
		if(!isset($this->conf['level'][$level]))
			$level = XIVO_TE_ERR_TRAC;

		if(is_null($report_type) || !isset($this->conf['rtype'][$report_type]))
			$report_type = $this->report_type;

		$context = (string) $context;

		if(!isset($this->conf['rmode'][$report_mode]))
			$report_type = $this->report_mode;

		if(is_array($msg) || is_object($msg) || is_bool($msg))
		{
			$tomsg = xivo_sprint_r($msg);
			if($report_type === XIVO_TE_RTYPE_SCREEN && $this->sapi === 'default')
				$tomsg = '<pre>'.$tomsg.'</pre>';
		}
		else $tomsg = (string) $msg;

		$tocolor = $this->conf['level'][$level]['color'];
		$tolabel = $this->conf['level'][$level]['label'];
		$tocontext = '('.$context.')';

		if($report_type & XIVO_TE_RTYPE_LOG)
		{
			$log_err = false;
			$file_err_log = ini_get('error_log');

			if(is_file($file_err_log) === true && is_writable($file_err_log) === true)
			{
				$log_err = true;
			}
			else if(is_file($file_err_log) === false && is_writable(dirname($file_err_log)) === true)
			{
				touch($file_err_log);
				$log_err = true;
			}

			if($log_err === true)
				error_log($tolabel.' '.$tocontext.' '.$tomsg,0);
		}

		if($report_type & XIVO_TE_RTYPE_SCREEN)
		{
			if($report_mode === XIVO_TE_RMODE_REALTIME)
			{
				if($this->sapi === 'default')
					echo '<span style="color: '.$tocolor.'";>'.$tomsg.'</span> '.$tocontext.'<br />';
				else
					echo 'Msg: '.$tomsg.' - Context: '.$tocontext."\n";
			}
			else
			{
				if($this->sapi === 'default')
					$this->delayed_content = sprintf('<tr><td style="color: %s">%s - %s</td><td>%s</td></tr>',$tocolor,$tolabel,$tocontext,$tomsg);
				else
					$this->delayed_content = sprintf('Label: %s - Context: %s - Msg: %s',$tolabel,$tocontext,$tomsg);
			}
		}

		if($report_type & XIVO_TE_RTYPE_MAIL && $this->mail !== false && xivo_chk_email($this->mail))
		{
			xivo_malert($tolabel, $tomsg, $this->mail);
		}
	}
}

?>
