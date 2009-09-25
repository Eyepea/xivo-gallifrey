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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_handynumbers extends xivo_application_asterisk_abstract
{
	var $_handynumbers	= false;
	var $_extenumbers	= false;
	var $_extensions	= false;
	var $_trunkfeatures	= false;
	var $_param		= array('type'	=> '');

	function xivo_application_service_asterisk_handynumbers(&$service,$param=array())
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_handynumbers = &$this->_service->get_module('handynumbers')) === false
		|| $this->load_init($param) === false
		|| ($this->_extenumbers = &$this->_service->get_module('extenumbers')) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_trunkfeatures = &$this->_service->get_module('trunkfeatures')) === false)
			return(false);

		return(true);
	}

	function load_init($param=array())
	{
		$this->_reset_info();
		$this->_reset_params();
		$this->_reset();

		if($this->set_params($param) === false
		|| isset($param['type']) === false
		|| $this->_handynumbers->chk_element_value('type',$param['type']) === false)
		{
			$this->_param['type'] = '';
			return(false);
		}

		$this->_param['type'] = $param['type'];

		return(true);
	}

	function get()
	{
		$this->_info = $this->_origin = array();

		$where = array();
		$where['type'] = $this->_param['type'];

		if(($this->_info['handynumbers'] = $this->_handynumbers->get_all_where($where)) === false
		|| $this->_get_all_extenumbers_exten() === false)
			return(false);

		$this->_origin['handynumbers'] = $this->_handynumbers->get_origin_list();

		return($this->_info);
	}

	function _get_all_extenumbers_exten()
	{
		$this->_info['extenumbers'] = false;
		$this->_origin['extenumbers'] = false;

		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		if(dwho_issa('handynumbers',$this->_info) === false
		|| empty($this->_info['handynumbers']) === true)
			return(false);

		$this->_info['extenumbers'] = array();
		$this->_origin['extenumbers'] = array();

		$where = array();
		$where['type'] = 'handynumbers';

		foreach($this->_info['handynumbers'] as $val)
		{
			$where['typeval'] = $val['id'];

			if(($info['extenumbers'] = $this->_extenumbers->get_where($where)) === false
			|| ($info['exten'] = $this->_extensions->get_exten($val['exten'],
									   XIVO_SRE_IPBX_AST_CONTEXT_HANDYNUMBERS)) === false)
				continue;

			$this->_info['extenumbers'][] = $info['extenumbers'];
			$this->_origin['extenumbers'][] = $this->_extenumbers->get_origin();

			$this->_info['exten'][] = $info['exten'];
			$this->_origin['exten'][] = $this->_extensions->get_origin();
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

	function _mk_exten_appdata($exten)
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_HANDYNUMBERS_MACRO;
		$arr['appdata'][1] = $this->_service->clean_extension($exten);

		return($arr);
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

			if(is_array($ref) === false || isset($ref['trunkfeaturesid'],$ref['exten']) === false)
				continue;
			else if(($ref['trunkfeaturesid'] = $this->_trunkfeatures->get_primary($ref['trunkfeaturesid'])) === false)
				$this->_set_error($j,'wrong trunk');

			if($this->set_extenumbers($ref['exten']) === false)
				$this->_set_error($j,$this->get_error('extenumbers'));

			if($this->set_exten($ref['exten']) === false)
				$this->_set_error($j,$this->get_error('exten'));

			$ref['type'] = $this->_param['type'];

			if(($rs = $this->_handynumbers->chk_values($ref)) === false)
			{
				$reslist[$j] = $this->_handynumbers->get_filter_result();
				$errlist[$j++] = $this->_handynumbers->get_filter_error();
				continue;
			}

			$result = $this->_handynumbers->get_filter_result();
			$reslist[$j++] = $result;

			if(dwho_issa('handynumbers',$this->_info) === true
			&& isset($this->_info['handynumbers'][$k]) === true)
				$result['id'] = $this->_info['handynumbers'][$k]['id'];

			$this->_set_result('handynumbers',$result);

			if(dwho_issa('extenumbers',$this->_info) === true
			&& isset($this->_info['extenumbers'][$k]) === true)
				$this->_set_result_var('extenumbers','id',$this->_info['extenumbers'][$k]['id']);

			if(dwho_issa('exten',$this->_info) === true
			&& isset($this->_info['exten'][$k]) === true)
				$this->_set_result_var('exten','id',$this->_info['exten'][$k]['id']);

			$k++;

			$this->save();
		}

		$this->_set_result('handynumbers',$reslist);

		return(true);
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

	function set_exten($exten)
	{
		$r = true;

		if(($rs = $this->_extensions->chk_exten($exten,
							XIVO_SRE_IPBX_AST_CONTEXT_HANDYNUMBERS,
							'macro',
							$this->_mk_exten_appdata($exten))) === false)
		{
			$r = false;
			$this->_set_error('exten',$this->_extensions->get_filter_error());
		}

		$this->_set_result('exten',$rs);

		return($r);
	}

	function save()
	{
		$r = true;

		$this->_status = 'save';
		$this->_return = array();
		$handynumbersid = null;

		if($this->get_errnb() > 0
		|| ($handynumbersid = $this->add_handynumbers()) === false
		|| $this->add_extenumbers($handynumbersid) === false
		|| $this->add_exten() === false)
		{
			$r = false;
			if($handynumbersid !== null)
				$this->_reverse_save();
		}

		$this->_status = '';

		return($r);
	}

	function add_handynumbers()
	{
		if(($rs = $this->get_result('handynumbers')) === null
		|| $rs === false)
			return(false);

		if(isset($rs['id']) === true)
			$gid = $rs['id'];
		else
			$gid = 0;

		unset($rs['id']);

		return(($this->_return['handynumbers'] = $this->_handynumbers->add($rs,$gid)));
	}

	function add_extenumbers($handynumbersid)
	{
		$rs = false;

		if(($handynumbersid = dwho_ulongint($handynumbersid)) === 0
		|| ($rs = $this->get_result('extenumbers')) === null
		|| $rs === false)
			return($rs);

		if(isset($rs['id']) === true)
			$eid = $rs['id'];
		else
			$eid = 0;

		unset($rs['id']);

		$rs['typeval'] = $handynumbersid;

		return(($this->_return['extenumbers'] = $this->_extenumbers->add($rs,$eid)));
	}

	function add_exten()
	{
		if(($rs = $this->get_result('exten')) === null
		|| $rs === false)
			return($rs);

		if(isset($rs['id']) === true)
			$eid = $rs['id'];
		else
			$eid = 0;

		unset($rs['id']);

		$this->_return['exten'] = $this->_extensions->add($rs,$eid);

		return($this->_return['exten']);
	}

	function _delete_all()
	{
		if(dwho_issa('extenumbers',$this->_info) === true
		&& ($nb = count($this->_info['extenumbers'])) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
				$this->_extenumbers->delete($this->_info['extenumbers'][$i]['id']);
		}

		if(dwho_issa('exten',$this->_info) === true
		&& ($nb = count($this->_info['exten'])) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
				$this->_extensions->delete($this->_info['exten'][$i]['id']);
		}

		$where = array();
		$where['type'] = $this->_param['type'];

		$this->_handynumbers->delete_where($where);
	}

	function _reverse_save()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'save'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'handynumbers':
					$this->_handynumbers->delete($val);
					break;
				case 'exten':
					$this->_extensions->delete($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->delete($val);
					break;
			}
		}

		return(true);
	}
}

?>
