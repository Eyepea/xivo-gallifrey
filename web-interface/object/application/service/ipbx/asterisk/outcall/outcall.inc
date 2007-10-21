<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_outcall extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_outcall		= null;
	var $_extenumbers	= null;
	var $_trunkfeatures	= null;
	var $_extensions	= null;
	var $_rightcall		= null;

	function xivo_application_service_asterisk_outcall(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_outcall = &$this->_service->get_module('outcall')) === false
		|| ($this->_extenumbers = &$this->get_abstract('extenumbers')) === false
		|| ($this->_trunkfeatures = &$this->_service->get_module('trunkfeatures')) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_rightcall = &$this->get_abstract('rightcall')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['outcall'] = $this->_outcall->get($id)) === false
		|| ($this->_info['extenumbers'] = $this->_get_extenumbers()) === false
		|| $this->_get_trunkfeatures() === false
		|| $this->_get_exten() === false)
			return(false);

		$this->_origin['outcall'] = $this->_outcall->get_origin();
		$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		$this->_get_rightcall();

		return($this->_info);
	}

	function _get_extenumbers()
	{
		return(parent::_get_extenumbers('outcall',$this->_info['outcall']['id']));
	}

	function _get_trunkfeatures()
	{
		$this->_info['tfeatures'] = false;
		$this->_origin['tfeatures'] = false;

		if($this->_info['outcall']['linked'] === false)
			return(null);

		if(($this->_info['tfeatures'] = $this->_trunkfeatures->get($this->_info['outcall']['trunkfeaturesid'])) === false)
			return(false);

		$this->_origin['tfeatures'] = $this->_trunkfeatures->get_origin();

		return($this->_info['tfeatures']);
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

		if(($this->_info['exten'] = $this->_extensions->get_all_where($exten_where)) === false)
			return(false);

		$this->_origin['exten'] = $this->_extensions->get_origin_list();

		return($this->_info['exten']);
	}

	function _get_rightcall()
	{
		$this->_info['rightcall'] = $this->_rightcall->get('outcall',$this->_info['outcall']['id']);

		return($this->_info['rightcall']);
	}

	function get_elements()
	{
		$r = array();
		$r['outcall'] = $this->_outcall->get_element();
		$r['extenumbers'] = $this->_extenumbers->get_element();

		return($r);
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
		|| xivo_issa('outcall',$arr) === false
		|| xivo_issa('extenumbers',$arr) === false
		|| isset($arr['extenumbers']['exten']) === false)
			return(false);

		$this->set_outcall($arr['outcall']);

		$context = (string) $this->get_result_var('outcall','context');

		$this->set_extenumbers($action,$arr['extenumbers']['exten'],$context);

		$exten = (string) $this->get_result_var('extenumbers','exten');

		if($action === 'edit')
		{
			$this->_set_result_var('exten','exten',$exten);
			$this->_set_result_var('exten','context',$context);
		}
		else
			$this->set_exten($exten,$context);

		if(xivo_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_outcall($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('outcall',null);
			$this->_set_error('outcall','invalid data');
			return(false);
		}

		$arr['linked'] = true;
		$arr['commented'] = false;

		if(($rs = $this->_outcall->chk_values($arr)) === false
		|| $this->_trunkfeatures->get_id($rs['trunkfeaturesid']) === false)
		{
			$this->_set_result('outcall',$this->_outcall->get_filter_result());
			$this->_set_error('outcall',$this->_outcall->get_filter_error());

			return(false);
		}

		$this->_set_result('outcall',$rs);

		return(true);
	}

	function set_extenumbers($action,$number,$context)
	{
		return(parent::set_extenumbers($action,'outcall',$number,$context));
	}

	function set_exten($exten,$context)
	{
		if(($rs = $this->_extensions->new_exten('macro',
						 array('appdata' => 'superoutcall'),
						 $exten,
						 $context)) === false
		|| $this->_extensions->set_exten('hangup',$rs) === false)
		{
			$this->_set_error('exten',false);
			return(false);
		}

		$this->_set_result('exten',$rs);

		return(true);
	}

	function set_rightcall($list)
	{
		if(($r = $this->_rightcall->set($list,'outcall')) === false)
			$this->_set_error('rightcall',$this->_rightcall->get_error());

		$this->_set_result('rightcall',$this->_rightcall->get_result());

		return($r);
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$outcallid = null;

		if($this->get_errnb() > 0
		|| ($outcallid = $this->add_outcall()) === false
		|| $this->add_extenumbers($outcallid) === false
		|| $this->add_exten() === false
		|| $this->add_rightcall($outcallid) === false)
		{
			if($outcallid !== null)
				$this->_reverse_add();

			$this->_status = '';

			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function add_outcall()
	{
		if(($rs = $this->get_result('outcall')) === null
		|| $rs === false)
			return($rs);

		$this->_return['outcall'] = $this->_outcall->add($rs);

		return($this->_return['outcall']);
	}

	function add_exten()
	{
		if(($rs = $this->get_result('exten')) === null
		|| $rs === false)
			return($rs);

		$this->_return['exten'] = $this->_extensions->add_exten($rs);

		return($this->_return['exten']);
	}

	function add_rightcall($outcallid)
	{
		$r = $this->_rightcall->add($outcallid);

		$this->_return['rightcall'] = $this->_rightcall->get_return();

		return($r);
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
				case 'exten':
					$this->_extensions->delete_exten($val);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
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
		|| $this->edit_outcall() === false
		|| $this->edit_extenumbers() === false
		|| $this->edit_exten() === false
		|| $this->edit_rightcall() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function edit_outcall()
	{
		$rs = false;

		if(xivo_issa('outcall',$this->_info) === false
		|| ($rs = $this->get_result('outcall')) === null
		|| $rs === false)
			return($rs);

		$rs['commented'] = $this->_info['outcall']['commented'];

		$this->_return['outcall'] = $this->_outcall->edit($this->_info['outcall']['id'],$rs);

		return($this->_return['outcall']);
	}

	function edit_extenumbers()
	{
		if(xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		return(parent::edit_extenumbers($this->_info['extenumbers']['id']));
	}

	function edit_exten()
	{
		$rs = false;

		if(xivo_issa('exten',$this->_info) === false
		|| xivo_issa('outcall',$this->_info) === false
		|| xivo_issa('extenumbers',$this->_info) === false
		|| ($rs = $this->get_result('exten')) === null
		|| $rs === false)
			return($rs);

		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

		$this->_return['exten'] = $this->_extensions->edit_where($exten_where,$rs);

		return($this->_return['exten']);
	}

	function edit_rightcall()
	{
		if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		$r = $this->_rightcall->edit($this->_status,'outcall',$this->_info['outcall']['id']);

		$this->_return['rightcall'] = $this->_rightcall->get_return();

		return($r);
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
				case 'exten':
					$this->_extensions->edit_origin_list($this->_origin['exten']);
					break;
				case 'rightcall':
					if(xivo_issa('outcall',$this->_info) === false)
						break;

					$this->_rightcall->reverse_edit($this->_status,'outcall',$this->_info['outcall']['id']);
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

		if($this->delete_outcall() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_exten() === false
		|| $this->delete_rightcall() === false)
		{
			$this->_reverse_delete();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function delete_outcall()
	{
		if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		$r = $this->_outcall->delete($this->_info['outcall']['id']);

		if($this->_status === 'delete')
			$this->_return['outcall'] = $r;

		return($r);
	}

	function delete_extenumbers()
	{
		if(xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		return(parent::delete_extenumbers($this->_info['extenumbers']['id']));
	}

	function delete_exten()
	{
		if(xivo_issa('exten',$this->_info) === false
		|| xivo_issa('outcall',$this->_info) === false
		|| xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

		$r = $this->_extensions->delete_where($exten_where);

		if($this->_status === 'delete')
			$this->_return['exten'] = $r;

		return($r);
	}

	function delete_rightcall()
	{
		if(xivo_issa('outcall',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['outcall']['id'];

		$r = $this->_rightcall->delete($this->_status,'outcall',$id);

		if($this->_status === 'delete')
			$this->_return['rightcall'] = $r;

		return($r);
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
				case 'exten':
					$this->_extensions->add_origin_list($this->_origin['exten']);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
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

		if($this->enable_outcall() === false
		|| $this->enable_exten() === false)
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

		if($this->disable_outcall() === false
		|| $this->disable_exten() === false)
		{
			$this->_reverse_enable_disable();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function enable_outcall()
	{
		return($this->_enable_disable_outcall(false));
	}

	function disable_outcall()
	{
		return($this->_enable_disable_outcall(true));
	}

	function _enable_disable_outcall($disable=false)
	{
		$disable = (bool) $disable;

		if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		if($disable === false)
			$r = $this->_outcall->enable($this->_info['outcall']['id']);
		else
			$r = $this->_outcall->disable($this->_info['outcall']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['outcall'] = $r;

		return($r);
	}

	function enable_exten()
	{
		return($this->_enable_disable_exten(false));
	}

	function disable_exten()
	{
		return($this->_enable_disable_exten(true));
	}

	function _enable_disable_exten($disable=false)
	{
		$disable = (bool) $disable;

		if(xivo_issa('exten',$this->_info) === false
		|| xivo_issa('outcall',$this->_info) === false
		|| xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

		if($disable === false)
			$r = $this->_extensions->enable_where($exten_where);
		else
			$r = $this->_extensions->disable_where($exten_where);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['exten'] = $r;

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
				case 'outcall':
					$this->_enable_disable_outcall($disable);
					break;
				case 'exten':
					$this->_enable_disable_exten($disable);
					break;
			}
		}
		return(true);
	}
}

?>
