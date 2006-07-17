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
	var $delayed_content = array();
	var $debug_func = 'print_r';
	var $sapi = 'default';
	var $mail = false;
	var $conf = array();

	function trac_err($conf,$params=array())
	{
		if(!is_array($conf))
			die('ERR : VAR CONF MUST BE AN ARRAY (error.inc)');

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

		if(isset($params['debug_func']) && ($params['debug_func'] === 'print_r' || $params['debug_func'] === 'var_dump'))
			$this->debug_func = $params['debug_func'];
		
		if(isset($params['mail']) && xivo_chk_email($params['mail']) === true)
			$this->mail = (string) $params['mail'];
		else if(defined('XIVO_ROOT_MAIL') && xivo_chk_email(XIVO_ROOT_MAIL) === true)
			$this->mail = (string) XIVO_ROOT_MAIL;
		else
			$this->mail = false;
	}

	function warning($msg,$context='',$report_type=null)
	{
		return($this->report_err($msg,XIVO_TE_ERR_WARNING,$context,$report_type));
	}

	function error($msg,$context='',$report_type=null)
	{
		return($this->report_err($msg,XIVO_TE_ERR_ERROR,$context,$report_type));
	}

	function notice($msg,$context='',$report_type=null)
	{
		return($this->report_err($msg,XIVO_TE_ERR_NOTICE,$context,$report_type));
	}

	function report_err($msg,$level=0,$context=XIVO_TE_CONTEXT,$report_type=null)
	{
		$level = (int) $level;

		if(($level & $this->level) === 0)
			return(false);

		if(is_null($report_type) || !isset($this->conf['rtype'][$report_type]))
			$report_type = $this->report_type;

		$context = $context !== '' ? (string) $context : XIVO_TE_CONTEXT;

		if($this->report_mode === XIVO_TE_RMODE_REALTIME || $report_type !== XIVO_TE_RTYPE_SCREEN)
		{
			return($this->display($msg,$level,$report_type,$context,$this->report_mode));
		}

		return($this->display($msg,XIVO_TE_ERR_TRAC,XIVO_TE_RTYPE_SCREEN,$context,XIVO_TE_RMODE_DELAYED));
	}

	function report_e_err($errno,$errstr,$errfile,$errline)
	{
		$context = XIVO_TE_CONTEXT;
		$level = XIVO_TE_ERR_TRAC;

		$msg = '['.$errno.'] '.$errstr.' - File: '.$errfile.' - Line: '.$errline;

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
		if(is_null($report_type) || !isset($this->conf['rtype'][$report_type]))
			$report_type = $this->report_type;


		if(!isset($this->conf['rmode'][$report_mode]))
			$report_mode = $this->report_mode;
		
		if($report_type === XIVO_TE_RTYPE_NONE)
			return(false);

		$context = $context !== '' ? (string) $context : XIVO_TE_CONTEXT;

		if($report_mode === XIVO_TE_RMODE_REALTIME || $report_type !== XIVO_TE_RTYPE_SCREEN)
		{
			return($this->display($msg,XIVO_TE_ERR_TRAC,$report_type,$context,$this->report_mode));
		}
		
		return($this->display($msg,XIVO_TE_ERR_TRAC,XIVO_TE_RTYPE_SCREEN,$context,XIVO_TE_RMODE_DELAYED));
	}

	function display($msg,$level=XIVO_TE_ERR_TRAC,$report_type=null,$context=XIVO_TE_CONTEXT,$report_mode=0)
	{
		if(!isset($this->conf['level'][$level]))
			$level = XIVO_TE_ERR_TRAC;

		if(is_null($report_type) || !isset($this->conf['rtype'][$report_type]))
			$report_type = $this->report_type;

		$context = (string) $context;

		if(strlen($context) === 0)
			$context = XIVO_TE_CONTEXT;

		if(!isset($this->conf['rmode'][$report_mode]))
			$report_mode = $this->report_mode;

		if($report_type === XIVO_TE_RTYPE_NONE)
			return(false);

		$tomsg_log = '';

		if(is_array($msg) || is_object($msg) || is_bool($msg) || is_null($msg))
		{
			if($this->debug_func === 'var_dump' || (is_bool($msg) || is_null($msg)))
			{
				$tomsg = xivo_svar_dump($msg);
			}
			else
			{
				$tomsg = xivo_sprint_r($msg);
			}

			if($report_type & XIVO_TE_RTYPE_LOG || $this->sapi !== 'default')
				$tomsg_log = $tomsg;

			if($report_type & XIVO_TE_RTYPE_SCREEN && $this->sapi === 'default')
				$tomsg = '<pre>'.$tomsg.'</pre>';
		}
		else $tomsg = (string) $msg;

		if($tomsg_log === '')
			$tomsg_log = $tomsg;

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
				error_log($tolabel.' '.$tocontext.' - Message: '.$tomsg_log,0);
		}

		if($report_type & XIVO_TE_RTYPE_SCREEN)
		{
			if($report_mode === XIVO_TE_RMODE_REALTIME)
			{
				if($this->sapi === 'default')
					echo '<span style="color: '.$tocolor.'";>'.$tomsg.'</span> '.$tocontext.'<br />';
				else
					echo $tolabel.' '.$tocontext.' - Message: '.$tomsg_log."\n";
			}
			else
			{
				if(!isset($this->delayed_content[$level]))
					$this->delayed_content[$level] = array();
					
				if($this->sapi === 'default')
					$this->delayed_content[$level][] = '<tr><td style="color: '.$tocolor.'">'.$tolabel.' - '.$tocontext.'</td><td>'.$tomsg.'</td></tr>'."\n";
				else
					$this->delayed_content[$level][] = $tolabel.' '.$tocontext.' - Message: '.$tomsg_log."\n";
			}
		}

		if($report_type & XIVO_TE_RTYPE_MAIL && $this->mail !== false && xivo_chk_email($this->mail) === true)
		{
			xivo_malert($tolabel, $tomsg, $this->mail);
		}
	}

	function display_delayed()
	{
		if(is_array($this->delayed_content))
		{
			ksort($this->delayed_content,SORT_NUMERIC);
			$k = array_keys($this->delayed_content);
			$cnt = count($k);

			if($cnt > 0)
			{
				echo '<table cellspacing="0" cellpadding="0" border="1">';
				for($i = 0;$i < $cnt;$i++)
				{
					if(!(is_array($this->delayed_content[$k[$i]]) && ($nb = count($this->delayed_content[$k[$i]])) !== 0))
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						echo $this->delayed_content[$k[$i]][$j];
					}
				}
				echo '</table>';
			}
		}
	}
}

?>
