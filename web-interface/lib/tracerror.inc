<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

class dwho_tracerror
{
	var $_param = array(
				'level'		=> 0,
				'report_type'	=> 0,
				'report_mode'	=> 0,
				'report_func'	=> 1,
				'email'		=> false,
				'file'		=> false);

	var $_delayed_content	= array();
	var $_sapi		= DWHO_SAPI_MODE;
	var $_conf		= array();

	function dwho_tracerror($param=array())
	{
		$param = (array) $param;

		if(($this->_conf = dwho_gat::get_file(DWHO_PATH_CONF.DWHO_SEP_DIR.'tracerror')) === false)
			die('Failed to load tracerror configuration array');

		if(isset($param['level']) === false
		|| $this->set_param('level',$param['level']) === false)
			$this->set_param('level',DWHO_TE_ERR_ALL);

		if(isset($param['report_type']) === false
		|| $this->set_param('report_type',$param['report_type']) === false)
			$this->set_param('report_type',DWHO_TE_RTYPE_LOG);

		if(isset($param['report_mode']) === false
		|| $this->set_param('report_mode',$param['report_mode']) === false)
			$this->set_param('report_mode',DWHO_TE_RMODE_REALTIME);

		if(isset($param['report_func']) === false
		|| $this->set_param('report_func',$param['report_func']) === false)
			$this->set_param('report_func',1);

		if(isset($param['email']) === false
		|| $this->set_param('email',$param['email']) === false)
			$this->_param['email'] = false;

		if((isset($param['file']) === false
		   || $this->set_param('file',$param['file']) === false) === true
		&& $this->set_param('file',ini_get('error_log')) === false)
			$this->_param['file'] = false;
	}

	function get_param($name)
	{
		if(isset($this->_param[$name]) === true)
			return($this->_param[$name]);

		return(null);
	}

	function set_param($name,$value)
	{
		switch($name)
		{
			case 'level':
				if(($value = $this->_chk_level($value)) === false)
					return(false);

				ini_set('error_reporting',$value);
				break;
			case 'report_type':
				if(($value = $this->_chk_report_type($value)) === false)
					return(false);

				ini_set('display_errors',(bool) ($value & DWHO_TE_RTYPE_SCREEN));
				break;
			case 'report_mode':
				if($this->_chk_report_mode($value) === false)
					return(false);

				$value = (int) $value;
				break;
			case 'report_func':
				if(isset($this->_conf['rfunc'][$value]) === false)
					return(false);

				$value = (int) $value;
				break;
			case 'email':
				if(($value = $this->_chk_email($value)) === false)
					return(false);
				break;
			case 'file':
				if(($value = $this->_chk_file($value)) === false)
					return(false);
				break;
			default:
				return(false);
		}

		$this->_param[$name] = $value;

		return(true);
	}

	function notice($msg,$context='',$report_type=null)
	{
		return($this->report_err($msg,DWHO_TE_ERR_NOTICE,$context,$report_type));
	}

	function warning($msg,$context='',$report_type=null)
	{
		return($this->report_err($msg,DWHO_TE_ERR_WARNING,$context,$report_type));
	}

	function error($msg,$context='',$report_type=null)
	{
		return($this->report_err($msg,DWHO_TE_ERR_ERROR,$context,$report_type));
	}

	function _chk_level($level)
	{
		$r = false;

		$level = intval($level);

		if($level === 0 || ($level & DWHO_TE_ERR_ALL) !== 0)
			$r = $level;

		return($r);
	}

	function _chk_report_type($type)
	{
		$r = false;

		$type = intval($type);

		if($type === 0 || ($type & DWHO_TE_RTYPE_ALL) !== 0)
			$r = $type;

		return($r);
	}

	function _chk_report_mode($mode)
	{
		return(isset($this->_conf['rmode'][$mode]));
	}

	function _chk_email($email)
	{
		$r = false;

		if($email !== false)
			$r = dwho_chk_email($email);

		return($r);
	}

	function _chk_file($file)
	{
		if(($filelog = dwho_file::is_f_rw($file)) !== false)
			$r = $filelog;
		else if(dwho_file::is_d_rwx($file,true) !== false && dwho_file::is_f($file) === false)
			$r = touch($file);
		else
			$r = false;

		return($r);
	}

	function report_err($msg,$level=0,$context=DWHO_TE_CONTEXT,$report_type=null)
	{
		$level = $this->_chk_level($level);

		if($level === 0 || $level === false || ($this->_param['level'] & $level) === 0)
			return(false);
		else if(($report_type = $this->_chk_report_type($report_type)) === false)
			$report_type = $this->_param['report_type'];

		$context = $context !== '' ? (string) $context : DWHO_TE_CONTEXT;

		if($this->_param['report_mode'] === DWHO_TE_RMODE_REALTIME
		|| $report_type !== DWHO_TE_RTYPE_SCREEN)
			return($this->display($msg,$level,$report_type,$context,$this->_param['report_mode']));

		return($this->display($msg,DWHO_TE_ERR_TRAC,DWHO_TE_RTYPE_SCREEN,$context,DWHO_TE_RMODE_DELAYED));
	}

	function report_e_err($errno,$errstr,$errfile,$errline)
	{
		if(error_reporting() < 1)
			return(null);

		$context = DWHO_TE_CONTEXT;
		$level = DWHO_TE_ERR_TRAC;

		$msg = '['.$errno.'] '.$errstr.' - File: '.$errfile.' - Line: '.$errline;

		if(isset($this->_conf['e_context'][$errno]) === true)
		{
			$context = $this->_conf['e_context'][$errno];

			if(($errno & DWHO_TE_ERR_ERROR) !== 0)
				$level = DWHO_TE_ERR_ERROR;
			else if(($errno & DWHO_TE_ERR_WARNING) !== 0)
				$level = DWHO_TE_ERR_WARNING;
			else if(($errno & DWHO_TE_ERR_NOTICE) !== 0)
				$level = DWHO_TE_ERR_NOTICE;
			else if(($errno & DWHO_TE_ERR_PARSE) !== 0)
				$level = DWHO_TE_ERR_PARSE;
		}

		$this->report_err($msg,$level,$context,$this->_param['report_type']);
	}

	function report_trac($msg,$context=DWHO_TE_CONTEXT,$report_type=null,$report_mode=0)
	{
		if(($report_type = $this->_chk_report_type($report_type)) === false)
			$report_type = $this->_param['report_type'];

		if($this->_chk_report_mode($report_mode) === false)
			$report_mode = $this->_param['report_mode'];

		if($report_type === DWHO_TE_RTYPE_NONE)
			return(false);

		$context = $context !== '' ? (string) $context : DWHO_TE_CONTEXT;

		if($report_mode === DWHO_TE_RMODE_REALTIME || $report_type !== DWHO_TE_RTYPE_SCREEN)
		{
			return($this->display($msg,DWHO_TE_ERR_TRAC,$report_type,$context,$this->_param['report_mode']));
		}

		return($this->display($msg,DWHO_TE_ERR_TRAC,DWHO_TE_RTYPE_SCREEN,$context,DWHO_TE_RMODE_DELAYED));
	}

	function display($msg,$level=DWHO_TE_ERR_TRAC,$report_type=null,$context=DWHO_TE_CONTEXT,$report_mode=0)
	{
		if(($level = $this->_chk_level($level)) === false)
			$level = DWHO_TE_ERR_TRAC;

		if(($report_type = $this->_chk_report_type($report_type)) === false)
			$report_type = $this->_param['report_type'];

		if($this->_chk_report_mode($report_mode) === false)
			$report_mode = $this->_param['report_mode'];

		$context = (string) $context;

		if(isset($context{0}) === false)
			$context = DWHO_TE_CONTEXT;

		if($report_type === DWHO_TE_RTYPE_NONE)
			return(false);

		$tomsg_log = '';

		if(is_array($msg) === true || is_object($msg) === true || is_bool($msg) === true || is_null($msg) === true)
		{
			if($this->_param['report_func'] === 2 || (is_bool($msg) === true || is_null($msg) === true) === true)
				$tomsg = dwho_svar_dump($msg);
			else
				$tomsg = dwho_sprint_r($msg);

			if(($report_type & DWHO_TE_RTYPE_LOG) !== 0 || $this->_sapi !== 'default')
				$tomsg_log = $tomsg;

			if(($report_type & DWHO_TE_RTYPE_SCREEN) !== 0 && $this->_sapi === 'default')
				$tomsg = '<pre>'.$tomsg.'</pre>';
		}
		else $tomsg = (string) $msg;

		if($tomsg_log === '')
			$tomsg_log = $tomsg;

		$tocolor = $this->_conf['level'][$level]['color'];
		$tolabel = $this->_conf['level'][$level]['label'];
		$tocontext = '('.$context.')';

		if(($report_type & DWHO_TE_RTYPE_LOG) !== 0 && $this->_param['file'] !== false
		&& ($file = dwho_file::is_f_rw($this->_param['file'])) !== false)
			error_log(strftime('%Y-%m-%d %H:%M:%S %z').' - '.$tolabel.' '.$tocontext.' - Message: '.dwho_notag($tomsg_log)."\n",3,$file);

		$delayed = false;

		if(($report_type & DWHO_TE_RTYPE_SCREEN) !== 0)
		{
			if($report_mode === DWHO_TE_RMODE_REALTIME)
			{
				if($this->_sapi === 'default')
					echo '<span style="color: '.$tocolor.'";>'.$tomsg.'</span> '.$tocontext.'<br />';
				else
					echo $tolabel.' '.$tocontext.' - Message: '.$tomsg_log."\n";
			}
			else
			{
				$delayed = true;

				if(isset($this->_delayed_content[$level]) === false)
					$this->_delayed_content[$level] = array();

				if($this->_sapi === 'default')
					$this->_delayed_content[$level][] = '<tr><td style="color: '.$tocolor.'">'.$tolabel.' - '.$tocontext.'</td>'.
									   '<td>'.$tomsg.'</td></tr>'."\n";
				else
					$this->_delayed_content[$level][] = $tolabel.' '.$tocontext.' - Message: '.$tomsg_log."\n";
			}
		}

		if(($report_type & DWHO_TE_RTYPE_MAIL) !== 0 && ($mail = $this->_chk_email($this->_param['email'])) !== false)
			dwho_malert($tolabel,$tomsg,$mail);

		if($delayed === false && ($level & DWHO_TE_ERR_ERROR) !== 0)
			die();
	}

	function display_delayed()
	{
		if(is_array($this->_delayed_content) === false
		|| empty($this->_delayed_content) === true)
			return(null);

		ksort($this->_delayed_content,SORT_NUMERIC);

		if($this->_sapi === 'default')
			echo '<table cellspacing="0" cellpadding="0" border="1">';

		reset($this->_delayed_content);

		while(list($key) = each($this->_delayed_content))
		{
			if(is_array($this->_delayed_content[$key]) === false
			|| ($nb = count($this->_delayed_content[$key])) === 0)
				continue;

			for($i = 0;$i < $nb;$i++)
				echo $this->_delayed_content[$key][$i];
		}

		if($this->_sapi === 'default')
			echo '</table>';
	}
}

?>
