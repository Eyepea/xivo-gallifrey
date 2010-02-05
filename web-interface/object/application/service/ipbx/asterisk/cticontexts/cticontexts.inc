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

class xivo_application_service_asterisk_cticontexts extends xivo_application_asterisk_abstract
{
	var $_cticontexts = false;

	function xivo_application_service_asterisk_cticontexts(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);
		if(($this->_cticontexts = &$this->_service->get_module('cticontexts')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function get_contexts_edit()
	{
		return(false);
	}

	function get_elements()
	{
		$r = array();
		$r['contexts'] = $this->_cticontexts->get_element();

		return($r);
	}

	function delete()
	{
		$r = true;

		$this->_contexts = 'delete';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->delete_contexts() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_contexts = '';

		return($r);
	}

	function delete_contexts()
	{
		if(dwho_issa('cticontexts',$this->_info) === false)
			return(false);

		$r = $this->_cticontexts->delete($this->_info['cticontexts']['id']);

		if($this->_contexts === 'delete')
			$this->_return['contexts'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->_contexts !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'contexts':
                    if(isset($this->_origin['contexts']) === true
                    && $this->_origin['contexts'] !== false)
                        $this->_cticontexts->add_origin($this->_origin['contexts']);
					break;
			}
		}
		return(true);
	}

	function enable()
	{
		$r = true;

		$this->_contexts = 'enable';
		$this->_return = array();

		if($this->enable_contexts() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_contexts = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_contexts = 'disable';
		$this->_return = array();

		if($this->disable_contexts() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_contexts = '';

		return(true);
	}

	function enable_contexts()
	{
		return($this->_enable_disable_contexts(false));
	}

	function disable_contexts()
	{
		return($this->_enable_disable_contexts(true));
	}

	function _enable_disable_contexts($disable=false)
	{
		if(dwho_issa('contexts',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_cticontexts->enable($this->_info['contexts']['id']);
		else
			$r = $this->_cticontexts->disable($this->_info['contexts']['id']);

		if($this->_contexts === 'enable' || $this->_contexts === 'disable')
			$this->_return['contexts'] = $r;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_contexts !== 'enable' && $this->_contexts !== 'disable') === true
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		$disable = $this->_contexts === 'enable';

		$this->_contexts = '';

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'contexts':
					$this->_enable_disable_contexts($disable);
					break;
			}
		}

		return(true);
	}

	function get_contexts_list($order=null,$limit=null,$assoc=false)
	{
		$arr = array();
		$rs = $this->_cticontexts->get_all();
		if($rs === false || ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_cticontexts->get_cnt();
			return(false);
		}

		$r = $info = array();
		for($i = 0;$i < $nb;$i++)
		{
			$info['cticontexts'] = $rs[$i];
			$r[$info['cticontexts']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function getcontexts($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		$this->_info['contexts'] = $this->_cticontexts->get($id);

		if($this->_components === false)
			return($this->_info);

		return($this->_info);
	}
	
	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();
		if(($action !== 'add' && $action !== 'edit') === true
		|| dwho_issa('contexts',$arr) === false)
			return(false);

		$this->set_contexts($arr['contexts']);

		return(($this->get_errnb() < 1));
	}

	function set_contexts($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('contexts',null);
			$this->_set_error('contexts','invalid data');
			return(false);
		}
		else if(($rs = $this->_cticontexts->chk_values($arr)) === false)
		{
			$this->_set_result('contexts',$this->_cticontexts->get_filter_result());
			$this->_set_error('contexts',$this->_cticontexts->get_filter_error());
			return(false);
		}

		$this->_set_result('contexts',$rs);

		return(true);
	}

	function get($id, $disable=null)
	{
		$this->_info = $this->_origin = array();
		$this->_info['cticontexts'] = $this->_cticontexts->get($id);

		if($this->_components === false)
			return($this->_info);

		return($this->_info);
	}

	function edit()
	{
		$r = true;

		$this->_contexts = 'edit';
		$this->_return = array();

		$n = $this->get_errnb();
		if($this->get_errnb() > 0
		|| $this->edit_contexts() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_contexts = '';

		return($r);
	}

    function edit_contexts()
    {
		$t = dwho_issa('cticontexts',$this->_info);
		$h = $this->get_result('contexts');

        if(dwho_issa('cticontexts',$this->_info) === false
        || ($rs = $this->get_result('contexts')) === null
        || $rs === false)
            return(false);

		$this->_return['contexts'] = $this->_cticontexts->edit($this->_info['cticontexts']['id'],$rs);
        return($this->_return['contexts']);
    }

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_contexts !== 'edit'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'presences':
					if(isset($this->_origin['presences']) === false
					|| $this->_origin['presences'] === false)
						$this->delete_presences();
					else
						$this->_ctipresences->edit_origin($this->_origin['presences']);
					break;
			}
		}
		return(true);
	}

	function add()
	{
		$r = true;

		$this->_contexts = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| ($contextsid = $this->add_contexts()) === false)
		{
			$r = false;
			if($contextsid !== null)
				$this->_reverse_add();
		}

		$this->_contexts = '';

		return($r);
	}

	function add_contexts()
	{
		if(($rs = $this->get_result('contexts')) === null
		|| $rs === false)
			return(false);

		$this->_return['contexts'] = $this->_cticontexts->add($rs);
		return($this->_return['contexts']);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_contexts !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'contexts':
					$this->_cticontexts->delete($val);
					break;
			}
		}

		return(true);
	}
}
?>
