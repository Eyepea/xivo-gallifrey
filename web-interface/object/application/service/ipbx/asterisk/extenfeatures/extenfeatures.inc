<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

class xivo_application_service_asterisk_extenfeatures extends xivo_application_asterisk_abstract
{
	var $_extensions	= false;
	var $_extenumbers	= false;
	var $_staticsip		= null;
	var $_conf		= array();
	var $_filter		= false;
	var $_context		= XIVO_SRE_IPBX_AST_CONTEXT_FEATURES;

	function xivo_application_service_asterisk_extenfeatures(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'extenfeatures'))) === false)
			return(false);

		$this->_load_config(dirname(__FILE__));

		return(true);
	}

	function get($name)
	{
		$this->_info = $this->_origin = array();

		$this->_get_extenfeatures($name);
		$this->_get_extenumbers($name);

		return($this->_info);
	}

	function get_all_by_context($disable=null)
	{
		$arr = array();
		$arr['context'] = $this->_context;

		return($this->_extensions->get_all_name_by_context($arr,$disable,true));
	}

	function _get_extenfeatures($name,$disable=null)
	{
		$this->_info['extenfeatures'] = false;
		$this->_origin['extenfeatures'] = false;

		$arr = array();
		$arr['name'] = $name;
		$arr['context'] = $this->_context;

		if(($this->_info['extenfeatures'] = $this->_extensions->get_all_by_name_context($arr,$disable)) !== false)
			$this->_origin['extenfeatures'] = $this->_extensions->get_origin_list();

		return($this->_info['extenfeatures']);
	}

	function get_config_exten()
	{
		if(dwho_issa('exten',$this->_conf) === false)
			return(false);

		return($this->_conf['exten']);
	}

	function get_elements()
	{
		return($this->get_element());
	}

	function _chk_extenfeatures_exists($name,$cmd='')
	{
		$name = (string) $name;
		$cmd = (string) $cmd;

		if(dwho_issa('exten',$this->_conf) === false
		|| dwho_issa($name,$this->_conf['exten']) === false)
			return(false);
		else if($cmd !== '' && isset($this->_conf['exten'][$name][$cmd]) === false)
			return(false);

		return(true);
	}

	function _mk_extenfeatures_appdata_vars($appdata,$exten)
	{
		if(is_scalar($exten) === false
		|| preg_match('/^[_]?([0-9\*#]+)/',$exten,$extmatch) !== 1)
			return($appdata);

		$vars = array('$<EXTNUM>' => '','$<EXTNUMLEN>' => 0);

		$extnum = $extmatch[1];
		$vars['$<EXTNUM>'] = $extnum;
		$vars['$<EXTNUMLEN>'] = strlen($extnum);

		if(is_string($appdata) === true)
			return(strtr($appdata,$vars));
		else if(is_array($appdata) === false)
			return($appdata);

		$nb = count($appdata);

		for($i = 0;$i < $nb;$i++)
			$appdata[$i] = strtr($appdata[$i],$vars);

		return($appdata);
	}

	function _mk_extenfeatures_dialplan($name,$val,$disable=null)
	{
		$name = (string) $name;
		$val = (string) $val;
		$disable = $disable === null ? $disable : intval((bool) $disable);

		if($this->_chk_extenfeatures_exists($name) === false)
			return(false);

		$arr = $this->_conf['exten'][$name];
		$arr['name'] = $name;
		$arr['commented'] = $disable;

		if(isset($arr['_line']) === false)
		{
			if(isset($arr['app']) === false)
				return(false);
			else if(isset($arr['appdata']) === true)
				$arr['appdata'] = $this->_mk_extenfeatures_appdata_vars($arr['appdata'],$val);

			return($this->_extensions->new_exten($val,$this->_context,$arr['app'],$arr));
		}

		unset($arr['_line']);

		$ref = &$this->_conf['exten'];

		if(is_array($ref[$name]) === false || ($nb = count($ref[$name]['_line'])) === 0)
			return(false);

		$lines = array_values($ref[$name]['_line']);
		$line = array_merge($arr,$lines[0]);

		if(isset($line['app']) === false)
			return(false);
		else if(isset($line['appdata']) === true)
			$line['appdata'] = $this->_mk_extenfeatures_appdata_vars($line['appdata'],$val);

		if(($id = $this->_extensions->new_exten($val,$this->_context,$line['app'],$line)) === false)
			return(false);

		for($i = 1;$i < $nb;$i++)
		{
			$line = array_merge($arr,$lines[$i]);

			if(isset($line['app']) === false)
				return(false);
			else if(isset($line['appdata']) === true)
				$line['appdata'] = $this->_mk_extenfeatures_appdata_vars($line['appdata'],$val);

			if($this->_extensions->set_exten($line['app'],$id,$line) === false)
				return(false);
		}

		return($id);
	}

	function set($arr)
	{
		$this->_reset();

		if(is_array($arr) === false || isset($arr['name']) === false)
			return(false);

		$this->get($arr['name']);

		if($this->get($arr['name']) !== false
		&& array_key_exists('commented',$arr) === false
		&& dwho_ak('commented',$this->_info['extenfeatures']) === true)
			$arr['commented'] = $this->_info['extenfeatures']['commented'];

		$this->set_extenfeatures($arr);

		$number = (string) $this->get_result_var('extenfeatures','exten');
		$context = '';

		$this->set_extenumbers('save',$number,$context);

		return(($this->get_errnb() < 1));
	}

	function set_extenfeatures($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['name'],$arr['exten']) === false
		|| $this->_chk_extenfeatures_exists(strval($arr['name'])) === false)
			return(false);

		$rs = array();
		$rs['name'] = $arr['name'];
		$rs['commented'] = null;

		if(isset($arr['commented']) === true)
			$rs['commented'] = (bool) $arr['commented'];

		if(($exten = $this->extenfeatures_chk_value($arr['name'],$arr['exten'])) === false)
		{
			if(($exten = $this->get_filter_result()) !== false)
				$rs['exten'] = $exten[$arr['name']];

			$this->_set_result('extenfeatures',$rs);
			$this->_set_error('extenfeatures',$this->get_filter_error());
			return(false);
		}

		$rs['exten'] = $exten;
		$this->_set_result('extenfeatures',$rs);

		return(true);
	}

	function save()
	{
		$r = true;

		$this->_status = 'save';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| ($name = $this->save_extenfeatures()) === false
		|| $this->save_extenumbers($name) === false
		|| ($name === 'vmusermsg' && $this->save_sip_vmexten() === false) === true)
		{
			$r = false;
			$this->_reverse_save();
		}

		$this->_status = '';

		return($r);
	}

	function save_extenfeatures()
	{
		if(isset($this->_info['extenfeatures']) === false)
			return(false);
		else if($this->_info['extenfeatures'] === false)
			return($this->_add_extenfeatures());
		else if(($rs = $this->get_result('extenfeatures')) === null
		|| is_array($rs) === false
		|| ($id = $this->_mk_extenfeatures_dialplan($rs['name'],$rs['exten'],$rs['commented'])) === false
		|| ($list = $this->_extensions->get_loaded_exten($id)) === false
		|| ($nb = count($list)) === 0)
			return(false);

		$this->delete_extenfeatures();

		$this->_return['extenfeatures'] = $rs['name'];

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_origin['extenfeatures'] !== false
			&& isset($this->_origin['extenfeatures'][$i]) === true)
				$eid = $this->_origin['extenfeatures'][$i]['id'];
			else
				$eid = 0;

			if($this->_extensions->add($list[$i],$eid) === false)
				return(false);
		}

		return($rs['name']);
	}

	function save_extenumbers($name)
	{
		if(($r = parent::save_extenumbers($name)) !== null)
			return($r);

		return(false);
	}

	function save_sip_vmexten()
	{
		if(is_object($this->_staticsip) === false
		&& ($this->_staticsip = &$this->_service->get_realstatic('staticsip')) === false)
		{
			$this->_set_error('extenfeatures','invalid staticsip module');
			return(false);
		}
		else if(($rs = $this->get_result('extenfeatures')) === false
		|| is_array($rs) === false
		|| isset($rs['name'],$rs['exten']) === false
		|| $rs['name'] !== 'vmusermsg')
		{
			$this->_set_error('extenfeatures','invalid data');
			return(false);
		}

		$where = array();
		$where['category'] = 'general';
		$where['var_name'] = 'vmexten';

		$value = array();
		$value['var_val'] = $this->_service->clean_extension($rs['exten']);

		return($this->_staticsip->edit_where($where,$value));
	}

	function _add_extenfeatures()
	{
		if(($rs = $this->get_result('extenfeatures')) === null
		|| $rs === false
		|| ($id = $this->_mk_extenfeatures_dialplan($rs['name'],$rs['exten'],$rs['commented'])) === false)
			return(false);
		else if($this->_extensions->add_exten($id) === false)
			$this->_return['extenfeatures'] = false;
		else
			$this->_return['extenfeatures'] = $rs['name'];

		return($this->_return['extenfeatures']);
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
				case 'extenfeatures':
					$this->delete_extenfeatures($val);

					if(isset($this->_origin['extenfeatures']) === true
					&& $this->_origin['extenfeatures'] !== false)
						$this->_extensions->add_origin_list($this->_origin['extenfeatures']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
			}
		}
	}

	function extenfeatures_chk_value($key,$value)
	{
		return(parent::chk_value($key,$this->_service->clean_extension($value)));
	}

	function extenfeatures_set_chk_value($key,$value)
	{
		return(parent::set_chk_value($key,$this->_service->clean_extension($value)));
	}

	function delete_extenfeatures($name='')
	{
		if(dwho_issa('extenfeatures',$this->_info) === true)
			$name = $this->_info['extenfeatures']['name'];
		else if(is_scalar($name) === false)
			return(false);

		$where = array();
		$where['name'] = $name;

		$r = $this->_extensions->delete_where($where);

		if($this->_status === 'delete')
			$this->_return['extenfeatures'] = $r;

		return($r);
	}
}

?>
