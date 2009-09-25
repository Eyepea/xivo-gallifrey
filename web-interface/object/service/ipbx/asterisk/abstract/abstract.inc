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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'abstract','libdatastorage.inc'));

class xivo_service_asterisk_abstract extends xivo_libdatastorage_abstract
{
	var $_conf	= array();
	var $_sre	= null;

	function get_name()
	{
		return($this->_name);
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'service',
					   'ipbx',
					   'asterisk',
					   $this->_name,
					   'config');

		if(($this->_conf = dwho_gat::get_file($dir)) === false)
			$this->_conf = array();
	}

	function mk_identity($arr)
	{
		if(isset($this->_identity) === false
		|| is_object($this->_identity) === false)
			return(false);

		return($this->_identity->mk_identity($arr));
	}

	function chk_dest_endcall($action,$actionarg1=null)
	{
		$r = array();

		switch($action)
		{
			case 'busy':
			case 'congestion':
				$r['action'] = $action;

				if(($actionarg1 = dwho_ufloat($actionarg1)) !== 0)
					$r['actionarg1'] = $actionarg1;
				return($r);
			case 'hangup':
				$r['action'] = $action;
				return($r);
		}

		return(false);
	}

	function chk_dest_application($action,$actionarg1,$actionarg2=null)
	{
		$r = array();

		switch($action)
		{
			case 'callbackdisa':
			case 'disa':
				if(xivo_service_asterisk::chk_context($actionarg2) === false)
					return(false);
				else if($actionarg1 === 'no-password'
				|| dwho_has_len($actionarg1) === false)
					$actionarg1 = 'no-password';
				else if(dwho_digit($actionarg1) === false
				|| strlen($actionarg1) > 40)
					return(false);

				$r['action'] = $action;
				$r['actionarg1'] = $actionarg1;
				$r['actionarg2'] = $actionarg2;
				return($r);
			case 'directory':
			case 'voicemailmain':
				if(xivo_service_asterisk::chk_context($actionarg1) === false)
					return(false);

				$r['action'] = $action;
				$r['actionarg1'] = $actionarg1;
				return($r);
			case 'faxtomail':
				if(dwho::load_class('dwho_filter') === false
				|| dwho_filter::chk_email($actionarg1) === false
				|| strlen($actionarg1) > 80)
					return(false);

				$r['action'] = $action;
				$r['actionarg1'] = $actionarg1;
				return($r);
			case 'vmauthenticate':
				if((dwho_has_len($actionarg2) === true
				   && $actionarg2 !== 's') === true
				|| ($rs = $this->chk_dest_voicemail($actionarg1)) === false)
					return(false);

				$r['action'] = $action;
				$r['actionarg1'] = $rs['actionarg1'];
				$r['actionarg2'] = $actionarg2;
				return($r);
		}

		return(false);
	}

	function chk_dest_extension($actionarg1,$actionarg2)
	{
		if(is_scalar($actionarg1) === false
		|| is_scalar($actionarg2) === false
		|| preg_match('/^[0-9#\*]{1,40}$/',$actionarg1) !== 1
		|| xivo_service_asterisk::chk_context($actionarg2) === false)
			return(false);

		$r = array();
		$r['action'] = 'extension';
		$r['actionarg1'] = (string) $actionarg1;
		$r['actionarg2'] = (string) $actionarg2;

		return($r);
	}

	function chk_dest_sound($dir,$actionarg1,$actionarg2,$getinfo=false)
	{
		if(($sounds = &$this->_sre->get_module('sounds')) === false)
			return(false);
		else if(is_bool($getinfo) === true || dwho_has_len($getinfo) === false)
			$getinfo = 'pathnoext';

		if(($list = $sounds->get_list($dir,$getinfo)) === false
		|| isset($list[$actionarg1]) === false)
			return(false);

		$r = array();
		$r['action'] = 'sound';
		$r['actionarg1'] = $actionarg1;

		if(dwho_has_len($actionarg2) === false)
			return($r);

		$r['actionarg2'] = '';

		if(strpos($actionarg2,'skip') !== false)
			$r['actionarg2'] .= 'skip';

		if(strpos($actionarg2,'noanswer') !== false)
			$r['actionarg2'] .= 'noanswer';

		if(strpos($actionarg2,'j') !== false)
			$r['actionarg2'] .= 'j';

		return($r);
	}

	function chk_dest_custom($val)
	{
		if(preg_match('/^(?!(try)?system\()[a-z]{3,}\((.*)\)$/i',$val,$match) !== 1)
			return(false);
		else if($match[1] === '' || ctype_print($match[1]) === true)
			return(true);

		return(false);
	}

	function chk_dest_agent($actionarg1,$actionarg2='',$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		$actionarg2 = dwho_has_len($actionarg2) === true ? dwho_ufloat($actionarg2) : '';

		return($this->_chk_dest('agentfeatures',
					'agent',
					$actionarg1,
					$actionarg2,
					$id,
					$disable,
					false,
					$chkcontext,
					$context));
	}

	function chk_dest_user($actionarg1,$actionarg2='',$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		$actionarg2 = dwho_has_len($actionarg2) === true ? dwho_ufloat($actionarg2) : '';

		return($this->_chk_dest('userfeatures',
					'user',
					$actionarg1,
					$actionarg2,
					$id,
					$disable,
					$chknumber,
					$chkcontext,
					$context));
	}

	function chk_dest_group($actionarg1,$actionarg2='',$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		return($this->_chk_dest_group_queue('group',
						    $actionarg1,
						    $actionarg2,
						    $id,
						    $disable,
						    $chknumber,
						    $chkcontext,
						    $context));
	}

	function chk_dest_queue($actionarg1,$actionarg2='',$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		return($this->_chk_dest_group_queue('queue',
						    $actionarg1,
						    $actionarg2,
						    $id,
						    $disable,
						    $chknumber,
						    $chkcontext,
						    $context));
	}

	function _chk_dest_group_queue($action,$actionarg1,$actionarg2='',$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		$actionarg1 = intval($actionarg1);
		$actionarg2 = dwho_has_len($actionarg2) === true ? dwho_ufloat($actionarg2) : '';

		if($action !== 'group' && $action !== 'queue')
			return(false);
		else if(($info = $this->_chk_dest($action.'features',
						  $action,
						  $actionarg1,
						  $actionarg2,
						  $id,
						  false,
						  $chknumber,
						  $chkcontext,
						  $context)) === false)
			return(false);

		if($disable !== null)
		{
			$module = $this->_sre->get_module($action.'features');

			if(($info = $module->get($actionarg1)) === false
			|| ($queue = $this->_sre->get_module('queue')) === false
			|| $queue->get($info['name'],$disable) === false)
				return(false);
		}

		return($info);
	}

	function chk_dest_meetme($actionarg1,$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		return($this->_chk_dest('meetmefeatures',
					'meetme',
					$actionarg1,
					null,
					$id,
					$disable,
					$chknumber,
					$chkcontext,
					$context));
	}

	function chk_dest_voicemail($actionarg1,$actionarg2='',$id=null,$disable=null)
	{
		if($id !== null)
			$id = dwho_ulongint($id);

		if(($module = $this->_sre->get_module('voicemail')) === false
		|| ($info = $module->get(intval($actionarg1),$disable)) === false
		|| ($id !== null && $id === $info['uniqueid']) === true)
			return(false);

		$r = array();
		$r['action'] = 'voicemail';
		$r['actionarg1'] = $info['uniqueid'];
		$r['actionarg2'] = '';

		if(dwho_has_len($actionarg2) === false)
			return($r);

		if(strpos($actionarg2,'b') !== false)
			$r['actionarg2'] .= 'b';

		if(strpos($actionarg2,'s') !== false)
			$r['actionarg2'] .= 's';

		if(strpos($actionarg2,'u') !== false)
			$r['actionarg2'] .= 'u';

		if(strpos($actionarg2,'j') !== false)
			$r['actionarg2'] .= 'j';

		return($r);
	}

	function chk_dest_schedule($actionarg1,$id=null,$disable=null)
	{
		return($this->_chk_dest('schedule',
					'schedule',
					$actionarg1,
					null,
					$id,
					$disable,
					false,
					false,
					null));
	}

	function chk_dest_voicemenu($actionarg1,$actionarg2=1,$id=null,$disable=null)
	{
		return($this->_chk_dest('voicemenu',
					'voicemenu',
					$actionarg1,
					dwho_uint($actionarg2,1),
					$id,
					$disable,
					false,
					false,
					null));
	}

	function _chk_dest($type,$action,$actionarg1,$actionarg2=null,$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		if($id !== null)
			$id = dwho_ulongint($id);

		if(($module = $this->_sre->get_module($type)) === false
		|| ($info = $module->get(intval($actionarg1),$disable)) === false
		|| ($id !== null && $id === $info['id']) === true
		|| ((bool) $chknumber === true && dwho_has_len($info['number']) === false) === true
		|| ((bool) $chkcontext === true && $info['context'] !== $context) === true)
			return(false);

		$r = array();
		$r['action'] = $action;
		$r['actionarg1'] = $info['id'];

		if($actionarg2 !== null)
			$r['actionarg2'] = $actionarg2;

		return($r);
	}
}

?>
