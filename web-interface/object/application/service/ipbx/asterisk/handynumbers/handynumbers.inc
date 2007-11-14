<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_handynumbers extends xivo_application_asterisk_abstract
{
	var $_status		= '';
	var $_handynumbers	= null;
	var $_extenumbers	= null;
	var $_trunkfeatures	= null;
	var $_param		= array('type'	=> '');

	function xivo_application_service_asterisk_handynumbers(&$service,$param=array())
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if($this->set_params($param) === false
		|| isset($param['type']) === false
		|| ($this->_handynumbers = &$this->_service->get_module('handynumbers')) === false
		|| $this->_handynumbers->chk_element_value('type',$param['type']) === false
		|| ($this->_extenumbers = &$this->_service->get_module('extenumbers')) === false
		|| ($this->_trunkfeatures = &$this->_service->get_module('trunkfeatures')) === false)
			return(false);

		$this->_param['type'] = $param['type'];

		return(true);
	}

	function get()
	{
		$this->_info = $this->_origin = array();

		$where = array();
		$where['type'] = $this->_param['type'];

		if(($this->_info['handynumbers'] = $this->_handynumbers->get_all_where($where)) === false
		|| $this->_get_extenumbers() === false)
			return(false);

		$this->_origin['handynumbers'] = $this->_handynumbers->get_origin_list();

		return($this->_info);
	}

	function _get_extenumbers()
	{
		$this->_info['extenumbers'] = false;
		$this->_origin['extenumbers'] = false;

		if(xivo_issa('handynumbers',$this->_info) === false
		|| ($arr = xivo_get_aks($this->_info['handynumbers'])) === false)
			return(false);

		$this->_info['extenumbers'] = array();
		$this->_origin['extenumbers'] = array();

		$extenumbers_where = array();
		$extenumbers_where['type'] = 'handynumbers';

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$extenumbers_where['typeval'] = $this->_info['handynumbers'][$i]['id'];

			if(($info = $this->_extenumbers->get_where($extenumbers_where)) === false)
				continue;

			$this->_info['extenumbers'][] = $info;
			$this->_origin['extenumbers'][] = $this->_extenumbers->get_origin();
		}

		return(true);
	}

	function get_elements()
	{
		$r = array();
		$r['handynumbers'] = $this->_handynumbers->get_element();
		$r['extenumbers'] = $this->_extenumbers->get_element();
		$r['trunkfeatures'] = $this->_trunkfeatures->get_element();

		return($r);
	}

	function set_extenumbers($number)
	{
		$exists = false;

		$extenum = array();
		$extenum['exten'] = $number;
		$extenum['type'] = 'handynumbers';

		if(($rs = $this->_extenumbers->chk_values($extenum)) === false
		|| ($exists = $this->_extenumbers->exists_type($rs)) !== false)
		{
			$this->_set_result('extenumbers',$this->_extenumbers->get_filter_result());

			if($exists === false)
				$this->_set_error('extenumbers',$this->_extenumbers->get_filter_error());
			else
				$this->_set_error('extenumbers','exists');

			return(false);
		}

		$this->_set_result('extenumbers',$rs);

		return(true);
	}

	function set_save_all($list)
	{
		$this->_status = 'save';

		if(is_array($list) === false)
			return(false);

		$list = array_values($list);

		$info = array();
		$info['type'] = $this->_param['type'];

		$reslist = $errlist = array();

		$this->_delete_all();

		if(($nb = count($list)) === 0)
			return(true);

		for($i = $j = $k = 0;$i < $nb;$i++)
		{
			$this->_errnb = 0;

			$ref = &$list[$i];

			if(isset($ref['trunkfeaturesid'],$ref['exten']) === false)
				continue;

			if(($ref['trunkfeaturesid'] = $this->_trunkfeatures->get_id($ref['trunkfeaturesid'])) === false)
				$this->_set_error($j,'wrong trunk');

			if($this->set_extenumbers($ref['exten']) === false)
				$this->_set_error($j,$this->get_error('extenumbers'));

			$ref['type'] = $this->_param['type'];

			if(($rs = $this->_handynumbers->chk_values($ref)) === false)
			{
				$reslist[$j] = $this->_handynumbers->get_filter_result();
				$errlist[$j++] = $this->_handynumbers->get_filter_error();
				continue;
			}
			
			$result = $this->_handynumbers->get_filter_result();
			$reslist[$j++] = $result;

			if(xivo_issa('handynumbers',$this->_info) === true
			&& isset($this->_info['handynumbers'][$k]) === true)
				$result['id'] = $this->_info['handynumbers'][$k]['id'];

			$this->_set_result('handynumbers',$result);

			if(xivo_issa('extenumbers',$this->_info) === true
			&& isset($this->_info['extenumbers'][$k]) === true)
				$this->_set_result_var('extenumbers','id',$this->_info['extenumbers'][$k]['id']);

			$k++;

			$this->save();
		}

		$this->_set_result('handynumbers',$reslist);

		return(true);
	}

	function save()
	{
		$this->_status = 'save';
		$this->_return = array();
		$handynumbersid = null;

		if($this->get_errnb() > 0
		|| ($handynumbersid = $this->add_handynumbers()) === false
		|| $this->add_extenumbers($handynumbersid) === false)
		{
			if($handynumbersid !== null)
				$this->_reverse_save();

			$this->_status = '';
			return(false);
		}
		
		$this->_status = '';
		return(true);
	}

	function add_handynumbers()
	{
		if(($rs = $this->get_result('handynumbers')) === null
		|| $rs === false)
			return($rs);

		if(isset($rs['id']) === true)
			$gid = $rs['id'];
		else
			$gid = 0;

		unset($rs['id']);

		$this->_return['handynumbers'] = $this->_handynumbers->add($rs,$gid);

		return($this->_return['handynumbers']);
	}

	function add_extenumbers($handynumbersid)
	{
		$rs = false;

		if(($handynumbersid = xivo_uint($handynumbersid)) === 0
		|| ($rs = $this->get_result('extenumbers')) === null
		|| $rs === false)
			return($rs);

		if(isset($rs['id']) === true)
			$eid = $rs['id'];
		else
			$eid = 0;

		unset($rs['id']);

		$rs['typeval'] = $handynumbersid;

		$this->_return['extenumbers'] = $this->_extenumbers->add($rs,$eid);

		return($this->_return['extenumbers']);
	}

	function _delete_all()
	{
		if(xivo_issa('extenumbers',$this->_info) === true
		&& ($nb = count($this->_info['extenumbers'])) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
				$this->_extenumbers->delete($this->_info['extenumbers'][$i]['id']);
		}

		$where = array();
		$where['type'] = $this->_param['type'];

		$this->_handynumbers->delete_where($where);
	}

	function _reverse_save()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'save'
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
				case 'handynumbers':
					$this->_handynumbers->delete($key);
					break;
				case 'extenumbers':
					$this->_extenumbers->delete($key);
					break;
			}
		}
		return(true);
	}
}

?>
