<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libdatastorage.inc');

class xivo_service_asterisk_abstract extends xivo_libdatastorage_abstract
{
	var $_conf	= array();
	var $_sre	= null;

	function get_name()
	{
		return($this->_name);
	}

	function &get_dso()
	{
		$r = false;

		if(isset($this->_dso) === true && is_object($this->_dso) === true)
			$r = &$this->_dso;

		return(($ref = &$r));
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$dir = array();
		$dir[] = XIVO_PATH_OBJECT;
		$dir[] = 'service';
		$dir[] = 'ipbx';
		$dir[] = 'asterisk';
		$dir[] = $this->_name;
		$dir[] = 'config';

		if(($this->_conf = xivo_gat::get_file($dir)) === false)
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
				
				if(($actionarg1 = xivo_ufloat($actionarg1)) !== 0)
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
				|| xivo_haslen($actionarg1) === false)
					$actionarg1 = 'no-password';
				else if(xivo_digit($actionarg1) === false
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
				if(xivo::load_class('xivo_filter') === false
				|| xivo_filter::chk_email($actionarg1) === false
				|| strlen($actionarg1) > 80)
					return(false);

				$r['action'] = $action;
				$r['actionarg1'] = $actionarg1;
				return($r);
			case 'vmauthenticate':
				if((xivo_haslen($actionarg2) === true
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

	function chk_dest_sound($dir,$actionarg1,$actionarg2,$getinfo=false)
	{
		if(($sounds = &$this->_sre->get_module('sounds')) === false)
			return(false);
		else if(is_bool($getinfo) === true || xivo_haslen($getinfo) === false)
			$getinfo = 'pathnoext';

		if(($list = $sounds->get_list($dir,$getinfo)) === false
		|| isset($list[$actionarg1]) === false)
			return(false);

		$r = array();
		$r['action'] = 'sound';
		$r['actionarg1'] = $actionarg1;

		if(isset($actionarg2{0}) === false)
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
		if(preg_match('/^(?!system\()[a-z]{3,}\((.*)\)$/i',$val,$match) !== 1)
			return(false);
		else if($match[1] === '' || ctype_print($match[1]) === true)
			return(true);

		return(false);
	}

	function chk_dest_user($actionarg1,$actionarg2='',$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		$actionarg2 = xivo_haslen($actionarg2) === true ? xivo_ufloat($actionarg2) : '';

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
		$actionarg2 = xivo_haslen($actionarg2) === true ? xivo_ufloat($actionarg2) : '';

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

	function chk_dest_voicemail($actionarg1,$id=null,$disable=null)
	{
		if($id !== null)
			$id = xivo_ulongint($id);

		if(($module = $this->_sre->get_module('voicemail')) === false
		|| ($info = $module->get(intval($actionarg1),$disable)) === false
		|| ($id !== null && $id === $info['uniqueid']) === true)
			return(false);

		$r = array();
		$r['action'] = 'voicemail';
		$r['actionarg1'] = $info['uniqueid'];

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
					xivo_uint($actionarg2,1),
					$id,
					$disable,
					false,
					false,
					null));
	}

	function _chk_dest($type,$action,$actionarg1,$actionarg2=null,$id=null,$disable=null,$chknumber=false,$chkcontext=false,$context=null)
	{
		if($id !== null)
			$id = xivo_ulongint($id);

		if(($module = $this->_sre->get_module($type)) === false
		|| ($info = $module->get(intval($actionarg1),$disable)) === false
		|| ($id !== null && $id === $info['id']) === true
		|| ((bool) $chknumber === true && xivo_haslen($info['number']) === false) === true
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
