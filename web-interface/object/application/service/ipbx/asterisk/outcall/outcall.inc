<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_outcall extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_outcall		= false;
	var $_extenumbers	= false;
	var $_outcalltrunk	= false;
	var $_extensions	= false;
	var $_rightcall		= false;
	var $_contextmember	= false;
	var $_contextlist	= array();

	function xivo_application_service_asterisk_outcall(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_outcall = &$this->_service->get_module('outcall')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'outcall'))) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_outcalltrunk = &$this->_service->get_module('outcalltrunk')) === false
		|| ($this->_rightcall = $this->get_abstract('rightcall',array('type' => 'outcall'))) === false
		|| ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'outcall'))) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['outcall'] = $this->_outcall->get($id,$disable)) === false
		|| ($this->_info['extenumbers'] = $this->_get_extenumbers($this->_info['outcall']['id'])) === false
		|| $this->_get_exten() === false)
			return(false);

		$this->_origin['outcall'] = $this->_outcall->get_origin();
		$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_outcalltrunk();
		$this->_get_rightcall($this->_info['outcall']['id']);
		$this->_get_contextmember($this->_info['outcall']['id']);

		return($this->_info);
	}

	function _get_outcalltrunk()
	{
		$this->_info['outcalltrunk'] = false;
		$this->_origin['outcalltrunk'] = false;

		$where = array();
		$where['outcallid'] = $this->_info['outcall']['id'];

		if(($this->_info['outcalltrunk'] = $this->_outcalltrunk->get_all_where($where)) === false)
			return(false);

		$this->_origin['outcalltrunk'] = $this->_outcalltrunk->get_origin_list();

		return($this->_info['outcalltrunk']);
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		$this->_info['exten'] = $this->_extensions->get_exten('macro',
								    $this->_info['outcall']['exten'],
								    $this->_info['outcall']['context']);

		$this->_origin['exten'] = $this->_extensions->get_origin();

		return($this->_info['exten']);
	}

	function get_elements()
	{
		$r = array();
		$r['outcall'] = $this->_outcall->get_element();
		$r['extenumbers'] = $this->_extenumbers->get_element();

		return($r);
	}

	function get_outcalls_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_outcall->get_all($disable,true,$order,$limit,$assoc)) !== false
		&& empty($rs) === true)
			$rs = false;

		$this->_cnt = $this->_outcall->get_cnt();

		return($rs);
	}

	function _mk_exten_appdata($exten)
	{
		$exten = $this->_service->clean_extension($exten);

		$arr = array();
		$arr['appdata'] = XIVO_SRE_IPBX_AST_OUTCALL_MACRO.'|'.$exten;

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
		|| xivo_issa('outcall',$arr) === false)
			return(false);

		$this->set_outcall($arr['outcall']);
	
		$number = (string) $this->get_result_var('outcall','exten');
		$context = (string) $this->get_result_var('outcall','context');

		if(($rsexten = $this->set_macro($action,'exten',$number,$this->_mk_exten_appdata($number),$context)) !== false)
			$this->_set_result('exten',$rsexten);

		$this->set_extenumbers($action,$number,$context);

		if(xivo_issa('outcalltrunk',$arr) === true)
			$this->set_outcalltrunk($arr['outcalltrunk']);
		else
			$this->_set_error('outcalltrunk','invalid data');

		if(xivo_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
	}

	function set_outcall($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('outcall',null);
			$this->_set_error('outcall','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_outcall->chk_values($arr)) === false)
		{
			$this->_set_result('outcall',$this->_outcall->get_filter_result());
			$this->_set_error('outcall',$this->_outcall->get_filter_error());
			return(false);
		}

		$this->_set_result('outcall',$rs);

		return(true);
	}

	function set_outcalltrunk($arr)
	{
		if(is_array($arr) === false || ($nb = count($arr)) === 0)
		{
			$this->_set_result('outcalltrunk',null);
			$this->_set_error('outcalltrunk','invalid data');
			return(false);
		}
		else if(($trunkfeatures = &$this->_service->get_module('trunkfeatures')) === false)
		{
			$this->_set_result('outcalltrunk',false);
			return(false);
		}

		$arr = array_values($arr);

		$info = array();
		$info['outcallid'] = 0;

		$reslist = $tmp = array();

		for($i = 0,$j = 0;$i < $nb;$i++)
		{
			$info['priority'] = $j;

			if(($info['trunkfeaturesid'] = $trunkfeatures->get_primary($arr[$i])) === false
			|| ($rs = $this->_outcalltrunk->chk_values($info)) === false
			|| isset($tmp[$rs['trunkfeaturesid']]) === true)
				continue;

			$tmp[$rs['trunkfeaturesid']] = 1;
			$reslist[$j++] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('outcalltrunk',false);
			return(false);
		}

		$this->_set_result('outcalltrunk',$reslist);
		return(true);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['outcall-context']['context'] = (string) $this->get_result_var('outcall','context');
		$rs['outcall-context']['typeval'] = 0;
		$rs['outcall-context']['varname'] = 'context';

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$outcallid = null;

		if($this->get_errnb() > 0
		|| ($outcallid = $this->add_outcall()) === false
		|| $this->add_extenumbers($outcallid) === false
		|| $this->add_macro('exten') === false
		|| $this->add_outcalltrunk($outcallid) === false
		|| $this->add_rightcall($outcallid) === false
		|| $this->add_contextmember($outcallid) === false)
		{
			$r = false;
			if($outcallid !== null)
				$this->_reverse_add($outcallid);
		}

		$this->_status = '';

		return($r);
	}

	function add_outcall()
	{
		if(($rs = $this->get_result('outcall')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['outcall'] = $this->_outcall->add($rs)));
	}

	function add_outcalltrunk($outcallid)
	{
		if(($outcallid = xivo_ulongint($outcallid)) === 0
		|| ($rs = $this->get_result('outcalltrunk')) === null
		|| $rs === false)
			return(false);

		$id = array();

		$nb = count($rs);

		$this->_return['outcalltrunk'] = array();

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['outcallid'] = $outcallid;

			$id['outcallid'] = $outcallid;

			if($this->_outcalltrunk->add($rs[$i]) === false)
				return(false);

			$this->_return['outcalltrunk'][] = $id;
		}

		if(isset($this->_return['outcalltrunk'][0]) === false)
		{
			$this->_return['outcalltrunk'] = false;
			return(false);
		}

		return(true);
	}

	function _reverse_add($outcallid)
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| ($outcallid = xivo_ulongint($outcallid)) === 0
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'outcall':
					$this->_outcall->delete($outcallid);
					break;
				case 'exten':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add($this->_status);
					break;
				case 'outcalltrunk':
					$this->_outcalltrunk->delete_where(array('outcallid' => $outcallid));
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
					break;
				case 'contextmember':
					$this->_contextmember->reverse_add();
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
		|| $this->edit_outcall() === false
		|| $this->edit_extenumbers() === false
		|| $this->edit_macro('exten') === false
		|| $this->edit_outcalltrunk() === false
		|| $this->edit_rightcall() === false
		|| $this->edit_contextmember() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_outcall()
	{
		if(xivo_issa('outcall',$this->_info) === false
		|| ($rs = $this->get_result('outcall')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['outcall']['commented'];

		return(($this->_return['outcall'] = $this->_outcall->edit($this->_info['outcall']['id'],$rs)));
	}

	function edit_outcalltrunk()
	{
		if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		$this->_return['outcalltrunk'] = $this->delete_outcalltrunk();

		return($this->add_outcalltrunk($this->_info['outcall']['id']));
	}

	function edit_rightcall()
	{
		if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		return(parent::edit_rightcall($this->_info['outcall']['id']));
	}

	function edit_contextmember()
	{
		if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['outcall']['id']));
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
				case 'outcall':
					if(isset($this->_origin['outcall']) === false
					|| $this->_origin['outcall'] === false)
						$this->delete_outcall();
					else
						$this->_outcall->edit_origin($this->_origin['outcall']);
					break;
				case 'outcalltrunk':
					$this->delete_outcalltrunk();

					if(isset($this->_origin['outcalltrunk']) === true
					&& $this->_origin['outcalltrunk'] !== false)
						$this->_outcalltrunk->add_origin_list($this->_origin['outcalltrunk']);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_edit($this->_status);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === false
					|| $this->_origin['exten'] === false)
						$this->delete_macro('exten');
					else
						$this->_extensions->edit_origin($this->_origin['exten']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_edit($this->_status);
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

		if($this->delete_outcall() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_macro('exten') === false
		|| $this->delete_outcalltrunk() === false
		|| $this->delete_rightcall() === false
		|| $this->delete_contextmember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
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

	function delete_outcalltrunk()
	{
		if(xivo_issa('outcalltrunk',$this->_info) === false)
			return(null);
		else if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		$r = $this->_outcalltrunk->delete_where(array('outcallid' => $this->_info['outcall']['id']));

		if($this->_status === 'delete')
			$this->_return['outcalltrunk'] = $r;

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
				case 'outcall':
					if(isset($this->_origin['outcall']) === true
					&& $this->_origin['outcall'] !== false)
						$this->_outcall->add_origin($this->_origin['outcall']);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === true
					&& $this->_origin['exten'] !== false)
						$this->_extensions->add_origin($this->_origin['exten']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'outcalltrunk':
					if(isset($this->_origin['outcalltrunk']) === true
					&& $this->_origin['outcalltrunk'] !== false)
						$this->_outcalltrunk->add_origin_list($this->_origin['outcalltrunk']);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'contextmember':
					$this->_contextmember->add_origin_list();
					break;
			}
		}

		return(true);
	}

	function enable()
	{
		$r = true;
		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_outcall() === false
		|| $this->enable_exten() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;
		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_outcall() === false
		|| $this->disable_exten() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return($r);
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
		if(xivo_issa('outcall',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
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
		if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		$exten_where = array();
		$exten_where['exten'] = $this->_info['outcall']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

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
