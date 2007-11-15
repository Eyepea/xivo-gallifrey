<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_incall extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_cnt		= 0;
	var $_incall		= null;
	var $_extenumbers	= null;
	var $_extensions	= null;

	function xivo_application_service_asterisk_incall(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_incall = &$this->_service->get_module('incall')) === false
		|| ($this->_extenumbers = &$this->get_abstract('extenumbers')) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['incall'] = $this->_incall->get($id,$disable)) === false
		|| ($this->_info['extenumbers'] = $this->_get_extenumbers()) === false
		|| $this->_get_localexten() === false)
			return(false);

		$this->_origin['incall'] = $this->_incall->get_origin();
		$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		return($this->_info);
	}

	function _get_extenumbers()
	{
		return(parent::_get_extenumbers('incall',$this->_info['incall']['id']));
	}

	function _get_localexten()
	{
		$this->_info['localexten'] = false;
		$this->_origin['localexten'] = false;

		$this->_info['localexten'] = $this->_extensions->get_exten('macro',
								    $this->_info['extenumbers']['exten'],
								    $this->_info['extenumbers']['context']);

		$this->_origin['localexten'] = $this->_extensions->get_origin();

		return($this->_info['localexten']);
	}

	function get_elements()
	{
		$r = array();

		$r['incall'] = $this->_incall->get_element();
		$r['extenumbers'] = $this->_extenumbers->get_element();

		return($r);
	}

	function get_cnt()
	{
		return(xivo_uint($this->_cnt));
	}

	function get_incalls_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_incalls('search',$search,$disable,$order,$limit,$assoc));
	}

	function get_incalls_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_incalls('list',null,$disable,$order,$limit,$assoc));
	}

	function _get_incalls($action,$search=null,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$action = (string) $action;
		$assoc = (bool) $assoc;

		$this->_cnt = 0;

		switch($action)
		{
			case 'search':
				$rs = $this->_incall->get_search($search,$disable,$order,$limit);
				break;
			default:
				$rs = $this->_incall->get_all($disable,true,$order,$limit);
		}

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['incall'] = $rs[$i];
			$info['type'] = false;

			if($info['incall']['linked'] === true
			&& ($info['type'] = $this->_incall->is_valid($info['incall']['type'],
								     $info['incall']['typeval'])) === false)
				continue;

			if($info['type'] === true)
				$info['type'] = false;

			$r[$info['incall']['id']] = $info;
		}

		$this->_cnt = $this->_incall->get_cnt();

		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function _mk_exten_appdata($exten)
	{
		$exten = $this->_service->clean_extension($exten);

		$arr = array();
		$arr['appdata'] = XIVO_SRE_IPBX_AST_INCALL_MACRO.'|'.$exten;

		return($arr);
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('incall',$arr) === false)
			return(false);

		$this->set_incall($arr['incall']);
	
		$number = (string) $this->get_result_var('incall','exten');
		$context = XIVO_SRE_IPBX_AST_INCALL_CONTEXT;

		$this->set_extenumbers($action,$number,$context);

		$appdata = $this->_mk_exten_appdata($number);

		if(($localexten = $this->set_macro($action,'localexten',$number,$appdata,$context)) === false)
			$this->_set_error('localexten');

		$this->_set_result('localexten',$localexten);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_incall($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('incall',null);
			$this->_set_error('incall','invalid data');

			return(false);
		}

		$arr['linked'] = true;
		$arr['commented'] = false;

		if(($rs = $this->_incall->chk_values($arr)) === false)
		{
			$this->_set_result('incall',$this->_incall->get_filter_result());
			$this->_set_error('incall',$this->_incall->get_filter_error());

			return(false);
		}

		$this->_set_result('incall',$rs);

		return(true);
	}

	function set_extenumbers($action,$number,$context)
	{
		return(parent::set_extenumbers($action,'incall',$number,$context));
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$incallid = null;

		if($this->get_errnb() > 0
		|| ($incallid = $this->add_incall()) === false
		|| $this->add_extenumbers($incallid) === false
		|| $this->add_macro('localexten') === false)
		{
			if($incallid !== null)
				$this->_reverse_add();

			$this->_status = '';

			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function add_incall()
	{
		if(($rs = $this->get_result('incall')) === null
		|| $rs === false)
			return($rs);

		$this->_return['incall'] = $this->_incall->add($rs);

		return($this->_return['incall']);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'localexten':
					$this->_extensions->delete_exten($val);
					break;
				default:
					if(($module = &$this->_service->get_module($key)) === false)
						continue;

					$module->delete($val);
			}
		}
		return(true);
	}

	function edit()
	{
		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_incall() === false
		|| $this->edit_extenumbers() === false
		|| $this->edit_macro('localexten') === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function edit_incall()
	{
		$rs = false;

		if(xivo_issa('incall',$this->_info) === false
		|| ($rs = $this->get_result('incall')) === null
		|| $rs === false)
			return($rs);

		$rs['commented'] = $this->_info['incall']['commented'];

		$this->_return['incall'] = $this->_incall->edit($this->_info['incall']['id'],$rs);

		return($this->_return['incall']);
	}

	function edit_extenumbers()
	{
		if(xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		return(parent::edit_extenumbers($this->_info['extenumbers']['id']));
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'localexten':
					$this->_extensions->edit_origin($this->_origin['localexten']);
					break;
				default:
					if(($module = &$this->_service->get_module($key)) === false)
						continue;

					if($this->_origin[$key] === false)
						$module->delete($val);
					else
						$module->edit_origin($this->_origin[$key]);
			}
		}
		return(true);
	}

	function delete()
	{
		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_incall() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_macro('localexten') === false)
		{
			$this->_reverse_delete();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function delete_incall()
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		$r = $this->_incall->delete($this->_info['incall']['id']);

		if($this->_status === 'delete')
			$this->_return['incall'] = $r;

		return($r);
	}

	function delete_extenumbers()
	{
		if(xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		return(parent::delete_extenumbers($this->_info['extenumbers']['id']));
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			if($this->_return[$key] === false)
				continue;

			switch($key)
			{
				case 'localexten':
					$this->_extensions->add_origin($this->_origin['localexten']);
					break;
				default:
					if(($module = &$this->_service->get_module($key)) === false)
						continue;

					if($this->_origin[$key] !== false)
						$module->add_origin($this->_origin[$key]);
			}
		}
		return(true);
	}

	function enable()
	{
		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_incall() === false
		|| $this->enable_localexten() === false)
		{
			$this->_reverse_enable_disable();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function disable()
	{
		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_incall() === false
		|| $this->disable_localexten() === false)
		{
			$this->_reverse_enable_disable();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function enable_incall()
	{
		return($this->_enable_disable_incall(false));
	}

	function disable_incall()
	{
		return($this->_enable_disable_incall(true));
	}

	function _enable_disable_incall($disable=false)
	{
		$disable = (bool) $disable;

		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		if($disable === false)
			$r = $this->_incall->enable($this->_info['incall']['id']);
		else
			$r = $this->_incall->disable($this->_info['incall']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['incall'] = $r;

		return($r);
	}

	function enable_localexten()
	{
		return($this->_enable_disable_localexten(false));
	}

	function disable_localexten()
	{
		return($this->_enable_disable_localexten(true));
	}

	function _enable_disable_localexten($disable=false)
	{
		$disable = (bool) $disable;

		if(xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['extenumbers']['context'];

		if($disable === false)
			$r = $this->_extensions->enable_where($exten_where);
		else
			$r = $this->_extensions->disable_where($exten_where);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['localexten'] = $r;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_status !== 'enable' && $this->_status !== 'disable') === true
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		if($this->_status === 'enable')
			$disable = true;
		else
			$disable = false;

		$this->_status = '';

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			if($this->_return[$key] === false)
				continue;

			switch($key)
			{
				case 'incall':
					$this->_enable_disable_incall($disable);
					break;
				case 'localexten':
					$this->_enable_disable_localexten($disable);
					break;
			}
		}
		return(true);
	}
}

?>
