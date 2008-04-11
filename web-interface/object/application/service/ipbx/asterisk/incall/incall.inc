<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_incall extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_incall		= false;
	var $_extenumbers	= false;
	var $_extensions	= false;
	var $_rightcall		= false;

	function xivo_application_service_asterisk_incall(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_incall = &$this->_service->get_module('incall')) === false
		|| ($this->_extenumbers = &$this->get_abstract('extenumbers')) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_rightcall = &$this->get_abstract('rightcall')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['incall'] = $this->_incall->get($id,$disable)) === false
		|| ($this->_info['extenumbers'] = $this->_get_extenumbers()) === false
		|| $this->_get_exten() === false)
			return(false);

		$this->_origin['incall'] = $this->_incall->get_origin();
		$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_rightcall();

		return($this->_info);
	}

	function _get_extenumbers()
	{
		return(parent::_get_extenumbers('incall',$this->_info['incall']['id']));
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		$this->_info['exten'] = $this->_extensions->get_exten('macro',
								      $this->_info['incall']['exten'],
								      $this->_info['incall']['context']);

		$this->_origin['exten'] = $this->_extensions->get_origin();

		return($this->_info['exten']);
	}

	function _get_rightcall()
	{
		$this->_info['rightcall'] = $this->_rightcall->get('incall',$this->_info['incall']['id']);

		return($this->_info['rightcall']);
	}

	function get_sound()
	{
		if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);

		return($sounds->get_list('playback',true));
	}

	function get_elements()
	{
		$r = array();

		$r['incall'] = $this->_incall->get_element();
		$r['extenumbers'] = $this->_extenumbers->get_element();

		return($r);
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

		switch($action)
		{
			case 'search':
				$rs = $this->_incall->search($search,$disable,$order,$limit);
				break;
			default:
				$rs = $this->_incall->get_all($disable,true,$order,$limit);
		}

		$this->_cnt = $this->_incall->get_cnt();

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['incall'] = $rs[$i];
			$info['type'] = false;

			if($info['incall']['linked'] === true
			&& ($info['type'] = $this->_incall->is_valid($info['incall']['type'],
								     $info['incall']['typeval'],
								     $info['incall']['applicationval'])) === false)
				$info['incall']['linked'] = false;

			if($info['type'] === true)
				$info['type'] = false;

			$r[$info['incall']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_destination_result()
	{
		return(parent::get_destination_result_for_display('incall'));
	}

	function get_destination_list()
	{
		$r = parent::get_destination_list();

		if(($sounds = &$this->_service->get_module('sounds')) !== false)
			$r['sounds'] = $sounds->get_list('playback',true);
		else
			$r['sounds'] = false;

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

		$arr['incall']['context'] = XIVO_SRE_IPBX_AST_INCALL_CONTEXT;

		$this->set_incall($arr['incall']);
	
		$number = (string) $this->get_result_var('incall','exten');
		$context = (string) $this->get_result_var('incall','context');

		if(($rsexten = $this->set_macro($action,'exten',$number,$this->_mk_exten_appdata($number),$context)) !== false)
			$this->_set_result('exten',$rsexten);

		$this->set_extenumbers($action,$number,$context);

		if(xivo_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		return(($this->get_errnb() < 1));
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

	function set_rightcall($list)
	{
		if(($r = $this->_rightcall->set($list,'incall')) === false)
			$this->_set_error('rightcall',$this->_rightcall->get_error());

		$this->_set_result('rightcall',$this->_rightcall->get_result());

		return($r);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$incallid = null;

		if($this->get_errnb() > 0
		|| ($incallid = $this->add_incall()) === false
		|| $this->add_extenumbers($incallid) === false
		|| $this->add_macro('exten') === false
		|| $this->add_rightcall($incallid) === false)
		{
			$r = false;
			if($incallid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_incall()
	{
		if(($rs = $this->get_result('incall')) === null
		|| $rs === false)
			return(false);

		if($this->_incall->is_valid($rs['type'],
					    $rs['typeval'],
					    $rs['applicationval']) === false)
			return(false);

		$this->_return['incall'] = $this->_incall->add($rs);

		return($this->_return['incall']);
	}

	function add_rightcall($incallid)
	{
		$r = $this->_rightcall->add($incallid);

		$this->_return['rightcall'] = $this->_rightcall->get_return();

		return($r);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'incall':
					$this->_incall->delete($val);
					break;
				case 'exten':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->delete($this->_status,$val);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
					break;
			}
		}

		return(true);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_incall() === false
		|| $this->edit_extenumbers() === false
		|| $this->edit_macro('exten') === false
		|| $this->edit_rightcall() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_incall()
	{
		if(xivo_issa('incall',$this->_info) === false
		|| ($rs = $this->get_result('incall')) === null
		|| $rs === false)
			return(false);

		if($this->_incall->is_valid($rs['type'],
					    $rs['typeval'],
					    $rs['applicationval']) === false)
			return(false);

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

	function edit_rightcall()
	{
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		$r = $this->_rightcall->edit($this->_status,'incall',$this->_info['incall']['id']);

		$this->_return['rightcall'] = $this->_rightcall->get_return();

		return($r);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'incall':
					if(isset($this->_origin['incall']) === false
					|| $this->_origin['incall'] === false)
						$this->delete_incall();
					else
						$this->_incall->edit_origin($this->_origin['incall']);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === false
					|| $this->_origin['exten'] === false)
						$this->delete_macro('exten');
					else
						$this->_extensions->edit_origin($this->_origin['exten']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === false
					|| $this->_origin['extenumbers'] === false)
						$this->delete_extenumbers();
					else
						$this->_extenumbers->edit_origin($this->_origin['extenumbers']);
					break;
				case 'rightcall':
					if(xivo_issa('incall',$this->_info) === true)
						$this->_rightcall->reverse_edit($this->_status,'incall',$this->_info['incall']['id']);
					break;
			}
		}
		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_incall() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_macro('exten') === false
		|| $this->delete_rightcall() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
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

	function delete_rightcall()
	{
		if(xivo_issa('incall',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['incall']['id'];

		$r = $this->_rightcall->delete($this->_status,'incall',$id);

		if($this->_status === 'delete')
			$this->_return['rightcall'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'incall':
					if(isset($this->_origin['incall']) === true
					&& $this->_origin['incall'] !== false)
						$this->_incall->add_origin($this->_origin['incall']);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === true
					&& $this->_origin['exten'] !== false)
						$this->_extensions->add_origin($this->_origin['exten']);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
			}
		}

		return(true);
	}

	function enable()
	{
		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_incall() === false
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

		if($this->disable_incall() === false
		|| $this->disable_exten() === false)
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
		if(xivo_issa('incall',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_incall->enable($this->_info['incall']['id']);
		else
			$r = $this->_incall->disable($this->_info['incall']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['incall'] = $r;

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
		if(xivo_issa('incall',$this->_info) === false)
			return(false);

		$exten_where = array();
		$exten_where['exten'] = $this->_info['incall']['exten'];
		$exten_where['context'] = $this->_info['incall']['context'];

		if((bool) $disable === false)
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
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		$disable = $this->_status === 'enable';

		$this->_status = '';

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'incall':
					$this->_enable_disable_incall($disable);
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
