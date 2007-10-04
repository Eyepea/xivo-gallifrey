<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_outcall extends xivo_application_asterisk_abstract
{
	var $_origin	= array();
	var $_status	= '';

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($outcall = &$this->_ipbx->get_module('outcall')) === false
		|| ($this->_info['outcall'] = $outcall->get($id)) === false
		|| ($extenumbers = &$this->_ipbx->get_module('extenumbers')) === false
		|| ($this->_info['extenumbers'] = $extenumbers->get($this->_info['outcall']['extenumid'])) === false
		|| $this->_get_trunkfeatures() === false
		|| $this->_get_exten() === false)
			return(false);

		$this->_origin['outcall'] = $outcall->get_origin();
		$this->_origin['extenumbers'] = $extenumbers->get_origin();

		$this->_get_rightcall();

		return($this->_info);
	}

	function _get_trunkfeatures()
	{
		$this->_info['tfeatures'] = false;
		$this->_origin['tfeatures'] = false;

		if($this->_info['outcall']['linked'] === false)
			return(null);

		if(($tfeatures = &$this->_ipbx->get_module('trunkfeatures')) === false
		|| ($this->_info['tfeatures'] = $tfeatures->get($this->_info['outcall']['trunkfeaturesid'])) === false)
			return(false);

		$this->_origin['tfeatures'] = $tfeatures->get_origin();

		return($this->_info['tfeatures']);
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

		if(($extensions = &$this->_ipbx->get_module('extensions')) === false
		|| ($this->_info['exten'] = $extensions->get_all_where($exten_where)) === false)
			return(false);

		$this->_origin['exten'] = $extensions->get_origin_list();

		return($this->_info['exten']);
	}

	function _get_rightcall()
	{
		$this->_info['rightcall'] = false;
		$this->_origin['rightcall'] = false;

		$where = array();
		$where['type'] = 'outcall';
		$where['typeval'] = $this->_info['outcall']['id'];

		if(($rightcallmember = &$this->_ipbx->get_module('rightcallmember')) === false
		|| ($this->_info['rightcall'] = $rightcallmember->get_all_where($where)) === false
		|| isset($this->_info['rightcall'][0]) === false)
			return(false);

		$this->_origin['rightcall'] = $rightcallmember->get_origin_list();

		return($this->_info['rightcall']);
	}

	function get_element()
	{
		$r = array();

		if(($outcall = &$this->_ipbx->get_module('outcall')) !== false)
			$r['outcall'] = $outcall->get_element();
		else
			$r['outcall'] = false;

		if(($extenumbers = &$this->_ipbx->get_module('extenumbers')) !== false)
			$r['extenumbers'] = $extenumbers->get_element();
		else
			$r['extenumbers'] = false;

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
		if(is_array($arr) === false
		|| ($outcall = &$this->_ipbx->get_module('outcall')) === false
		|| ($tfeatures = &$this->_ipbx->get_module('trunkfeatures')) === false)
		{
			$this->_set_result('outcall',null);
			$this->_set_error('outcall','invalid module');
			return(false);
		}

		$arr['extenumid'] = 0;
		$arr['linked'] = true;
		$arr['commented'] = false;

		if(($rs = $outcall->chk_values($arr)) === false
		|| $tfeatures->get_id($rs['trunkfeaturesid']) === false)
		{
			$this->_set_result('outcall',$outcall->get_filter_result());
			$this->_set_error('outcall',$outcall->get_filter_error());

			return(false);
		}

		$this->_set_result('outcall',$rs);

		return(true);
	}

	function set_exten($exten,$context)
	{
		if(($extensions = &$this->_ipbx->get_module('extensions')) === false)
		{
			$this->_set_result('exten',null);
			$this->_set_error('exten','invalid module');
			return(false);
		}

		if(($rs = $extensions->new_exten('macro',
						 array('appdata' => 'superoutcall'),
						 $exten,
						 $context)) === false
		|| $extensions->set_exten('hangup',$rs) === false)
		{
			$this->_set_error('exten',false);
			return(false);
		}

		$this->_set_result('exten',$rs);

		return(true);
	}

	function set_rightcall($list)
	{
		if(is_array($list) === false
		|| ($rightcallmember = &$this->_ipbx->get_module('rightcallmember')) === false
		|| ($rightcall = &$this->_ipbx->get_module('rightcall')) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = array();
		$info['rightcallid'] = 0;
		$info['type'] = 'outcall';
		$info['typeval'] = 0;
		
		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($rightcallid = $rightcall->get_id($list[$i])) === false)
			{
				$this->_set_error('rightcall','wrong rightcall');
				continue;
			}

			$info['rightcallid'] = $rightcallid;

			if(($rs = $rightcallmember->chk_values($info)) === false
			|| isset($tmp[$rightcallid]) === true)
				continue;

			$tmp[$rightcallid] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('rightcall',false);
			return(false);
		}

		$this->_set_result('rightcall',$reslist);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$extenumid = null;

		if($this->get_errnb() > 0
		|| ($extenumid = $this->add_extenumbers()) === false
		|| ($outcallid = $this->add_outcall($extenumid)) === false
		|| $this->add_exten() === false
		|| $this->add_rightcall($outcallid) === false)
		{
			if($extenumid !== null)
				$this->_reverse_add();

			$this->_status = '';

			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function add_outcall($extenumid)
	{
		if(($extenumid = xivo_uint($extenumid)) === 0)
			return(false);

		if(($rs = $this->get_result('outcall')) === null
		|| $rs === false)
			return($rs);

		$outcall = &$this->_ipbx->get_module('outcall');

		$rs['extenumid'] = $extenumid;

		$this->_return['outcall'] = $outcall->add($rs);

		return($this->_return['outcall']);
	}

	function add_exten()
	{
		if(($rs = $this->get_result('exten')) === null
		|| $rs === false)
			return($rs);

		$extensions = &$this->_ipbx->get_module('extensions');

		$this->_return['exten'] = $extensions->add_exten($rs);

		return($this->_return['exten']);
	}

	function add_rightcall($outcallid)
	{
		if(($outcallid = xivo_uint($outcallid)) === 0)
			return(false);

		if(($rs = $this->get_result('rightcall')) === null || $rs === false)
			return($rs);

		$rightcallmember = &$this->_ipbx->get_module('rightcallmember');

		$this->_return['rightcall'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['typeval'] = $outcallid;

			if(($this->_return['rightcall'][] = $rightcallmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return['rightcall'][0]) === false)
			$this->_return['rightcall'] = false;

		return(true);
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
					$extensions = &$this->_ipbx->get_module('extensions');
					$extensions->delete_exten($val);
					break;
				case 'rightcall':
					$rightcallmember = &$this->_ipbx->get_module('rightcallmember');

					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$rightcallmember->delete($val[$j]);
					}
					break;
				default:
					if(($module = &$this->_ipbx->get_module($key)) === false)
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
		|| xivo_issa('extenumbers',$this->_info) === false
		|| ($rs = $this->get_result('outcall')) === null
		|| $rs === false)
			return($rs);

		$rs['extenumid'] = $this->_info['extenumbers']['id'];
		$rs['commented'] = $this->_info['outcall']['commented'];

		$outcall = &$this->_ipbx->get_module('outcall');
		$this->_return['outcall'] = $outcall->edit($this->_info['outcall']['id'],$rs);

		return($this->_return['outcall']);
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

		$extensions = &$this->_ipbx->get_module('extensions');
		
		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

		$this->_return['exten'] = $extensions->edit_where($exten_where,$rs);

		return($this->_return['exten']);
	}

	function edit_rightcall()
	{
		if(xivo_issa('outcall',$this->_info) === false)
			return(false);

		if(xivo_issa('rightcall',$this->_info) === false)
			return($this->add_rightcall($this->_info['outcall']['id']));

		$this->_return['rightcall'] = $this->delete_rightcall();

		if(($rs = $this->get_result('rightcall')) === null)
			return($this->_return['rightcall']);

		if($rs === false)
			return($rs);

		$rightcallmember = &$this->_ipbx->get_module('rightcallmember');

		$this->_return['rightcall'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['typeval'] = $this->_info['outcall']['id'];

			if(($this->_return['rightcall'][] = $rightcallmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return['rightcall'][0]) === false)
			$this->_return['rightcall'] = false;

		return(true);
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
					$extensions = &$this->_ipbx->get_module('extensions');
					$extensions->edit_origin_list($this->_origin['exten']);
					break;
				case 'rightcall':
					$this->delete_rightcall();

					$rightcallmember = &$this->_ipbx->get_module('rightcallmember');
					$rightcallmember->add_origin_list($this->_origin['rightcall']);
					break;
				default:
					if(($module = &$this->_ipbx->get_module($key)) === false)
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
		if(xivo_issa('outcall',$this->_info) === false
		|| ($outcall = &$this->_ipbx->get_module('outcall')) === false)
			return(false);

		$r = $outcall->delete($this->_info['outcall']['id']);

		if($this->_status === 'delete')
			$this->_return['outcall'] = $r;

		return($r);
	}

	function delete_exten()
	{
		if(xivo_issa('exten',$this->_info) === false
		|| xivo_issa('outcall',$this->_info) === false
		|| xivo_issa('extenumbers',$this->_info) === false
		|| ($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

		$r = $extensions->delete_where($exten_where);

		if($this->_status === 'delete')
			$this->_return['exten'] = $r;

		return($r);
	}

	function delete_rightcall()
	{
		if(xivo_issa('rightcall',$this->_info) === false)
			return(null);

		if(xivo_issa('outcall',$this->_info) === false
		|| ($rightcallmember = &$this->_ipbx->get_module('rightcallmember')) === false)
			return(false);

		$rightcallmember_where = array();
		$rightcallmember_where['type'] = 'outcall';
		$rightcallmember_where['typeval'] = $this->_info['outcall']['id'];

		$r = $rightcallmember->delete_where($rightcallmember_where);

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
					$extensions = &$this->_ipbx->get_module('extensions');
					$extensions->add_origin_list($this->_origin['exten']);
					break;
				case 'rightcall':
					$rightcallmember = &$this->_ipbx->get_module('rightcallmember');
					$rightcallmember->add_origin_list($this->_origin['rightcall']);
					break;
				default:
					if(($module = &$this->_ipbx->get_module($key)) === false)
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

		if(xivo_issa('outcall',$this->_info) === false
		|| ($outcall = &$this->_ipbx->get_module('outcall')) === false)
			return(false);

		if($disable === false)
			$r = $outcall->enable($this->_info['outcall']['id']);
		else
			$r = $outcall->disable($this->_info['outcall']['id']);

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
		|| xivo_issa('extenumbers',$this->_info) === false
		|| ($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		$exten_where = array();
		$exten_where['exten'] = $this->_info['extenumbers']['exten'];
		$exten_where['context'] = $this->_info['outcall']['context'];

		if($disable === false)
			$r = $extensions->enable_where($exten_where);
		else
			$r = $extensions->disable_where($exten_where);

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
