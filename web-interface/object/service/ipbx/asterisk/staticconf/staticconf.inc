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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_staticconf extends xivo_service_asterisk_abstract
{
	var $_staticconf	= array();

	function _load_staticconf_config()
	{
		if(empty($this->_staticconf) === false)
			return(null);

		$dir = dirname(__FILE__).DWHO_SEP_DIR.'config';

		if(($this->_staticconf['conf'] = dwho_gat::get_file($dir,false)) === false)
			$this->_staticconf = array();
	}

	function get($id,$disable=null)
	{
		if(($id = dwho_ulongint($id)) === 0
		|| ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function get_where($arr,$disable=null)
	{
		$arr = (array) $arr;

		$where = array();
		$where['filename'] = $this->_filename;

		if(isset($arr['id']) === true)
			$where['id'] = dwho_ulongint($arr['id']);

		if(isset($arr['category']) === true)
			$where['category'] = $arr['category'];

		if(isset($arr['var_name']) === true)
			$where['var_name'] = $arr['var_name'];

		if(isset($arr['var_val']) === true)
			$where['var_val'] = $arr['var_val'];

		if(isset($arr['cat_metric']) === true)
			$where['cat_metric'] = $arr['cat_metric'];

		if(isset($arr['var_metric']) === true)
			$where['var_metric'] = $arr['var_metric'];

		if(($id = $this->_dso->get_where($where)) === false)
			return(false);

		return($this->get($id,$disable));
	}

	function get_category($category,$disable=null)
	{
		if(($list = $this->get_list_where(array('category' => $category),$disable)) === false)
			return(false);

		$r = array();
		$r['category'] = $category;

		$nb = count($list);

		for($i = 0;$i < $nb;$i++)
		{
			$id = &$list[$i];

			if(($info = $this->get($id)) !== false)
				$r[$info['var_name']] = $info['var_val'];
		}

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		if(($id = dwho_ulongint($id)) !== 0)
			$r['id'] = $id;

		$r['filename'] = $this->_filename;
		$r['category'] = $arr['category'];
		$r['var_name'] = $arr['var_name'];
		$r['var_val'] = $arr['var_val'];

		if(isset($arr['cat_metric']) === true)
			$r['cat_metric'] = (int) $arr['cat_metric'];

		if(isset($arr['var_metric']) === true)
			$r['var_metric'] = (int) $arr['var_metric'];

		if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];
		else
			$r['commented'] = $arr['var_val'] === null;

		return($this->_dso->add($r));
	}

	function add_category($category,$arr)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		$r = $add = array();

		$add['category'] = $category;

		$i = 0;

		foreach($arr as $key => $val)
		{
			$add['var_name'] = $key;
			$add['var_val'] = $val;

			if(($id = $this->add($add)) === false)
				break;

			$r[$i++] = $id;
		}

		if(isset($r[0]) === false)
			return(false);
		else if($id !== false)
			return($r);

		for($j = 0;$j < $i;$j++)
			$this->delete($r[$j]);

		return(false);
	}

	function add_name($name,$arr,$id=0)
	{
		if(isset($arr['var_name']) === false)
			$arr['var_name'] = $name;

		return($this->add($arr,$id));
	}

	function add_name_val($name,$val,$id=0,$disable=null)
	{
		$arr = array('var_val' => $val);

		if($disable !== null)
			$arr['commented'] = (bool) $disable;

		return($this->add_name($name,$arr,$id));
	}

	function edit($id,$arr)
	{
		if(($id = dwho_ulongint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = array();

		$r['filename'] = $this->_filename;

		if(isset($arr['category']) === true)
			$r['category'] = $arr['category'];

		if(isset($arr['var_name']) === true)
			$r['var_name'] = $arr['var_name'];

		if(array_key_exists('var_val',$arr) === true)
			$r['var_val'] = $arr['var_val'];

		if(isset($arr['cat_metric']) === true)
			$r['cat_metric'] = (int) $arr['cat_metric'];

		if(isset($arr['var_metric']) === true)
			$r['var_metric'] = (int) $arr['var_metric'];

		if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];
		else
			$r['commented'] = dwho_ak('var_val',$arr,true,true) === null;

		return($this->_dso->edit($id,$r));
	}

	function edit_name($id,$name,$arr)
	{
		if(isset($arr['var_name']) === false)
			$arr['var_name'] = $name;

		return($this->edit($id,$arr));
	}

	function edit_name_val($id,$name,$val,$disable=null)
	{
		$arr = array('var_val' => $val);

		if($disable !== null)
			$arr['commented'] = (bool) $disable;

		return($this->edit_name($id,$name,$arr));
	}

	function edit_category($category,$arr)
	{
		if(is_array($arr) === false || isset($arr['category']) === false)
			return(false);

		$r = $where = $edit = array();

		$where['category'] = $category;
		$edit['category'] = $arr['category'];
		unset($arr['category']);

		if(empty($arr) === true)
			return(false);

		foreach($arr as $key => $val)
		{
			$where['var_name'] = $key;

			$edit['var_name'] = $key;
			$edit['var_val'] = $val;

			if(($info = $this->get_where($where)) === false)
				continue;
			else if(($ret = $this->edit($info['id'],$edit)) === false)
				break;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			return(false);
		else if($ret !== false)
			return($r);

		$nb = count($r);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$r[$i]['id'];
			$this->edit($ref['id'],$ref);
		}

		return(false);
	}

	function edit_where($arr,$val,$disable=null)
	{
		$arr = (array) $arr;

		$arr['filename'] = $this->_filename;

		if(is_array($val) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($values = $this->chk_values($val,true,false,false)) === false)
			return(false);

		return($this->_dso->edit_where($where,$values,$disable));
	}

	function replace_by_name($name,$arr)
	{
		if(isset($arr['var_name']) === false)
			$arr['var_name'] = $name;

		if(($rs = $this->get_where(array('var_name' => $name))) === false)
			$r = $this->add($arr);
		else
			$r = $this->edit($rs['id'],$arr);

		return($r);
	}

	function replace_val_by_name($name,$val)
	{
		$arr = array('var_val' => $val);

		return($this->replace_by_name($name,$arr));
	}

	function enable($id)
	{
		if(($r = $this->get($id,true)) === false)
			return(false);

		$r['commented'] = false;

		return($this->edit($r['id'],$r));
	}

	function disable($id)
	{
		if(($r = $this->get($id,false)) === false)
			return(false);

		$r['commented'] = true;

		return($this->edit($r['id'],$r));
	}

	function delete($id)
	{
		if(($id = dwho_ulongint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function delete_where($arr)
	{
		$arr = (array) $arr;

		if(($where = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->delete_where($where));
	}

	function delete_by_category($category)
	{
		return($this->_dso->delete_where(array('category' => $category)));
	}

	function get_list($disable=null)
	{
		if(($r = $this->_dso->get_list($disable)) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;

		unset($arr['commented']);

		$arr['filename'] = $this->_filename;

		if(($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get_list_where($where,$disable)) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_name_val($arr=array(),$disable=null)
	{
		if(($list = $this->_prepare_get_all_where($arr,$disable)) === false)
			return(false);

		$r = array();
		$nb = count($list);

		for($i = 0;$i < $nb;$i++)
			$r[$list[$i]['var_name']] = $list[$i]['var_val'];

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_name_val_by_category($arr=array(),$disable=null)
	{
		if(($list = $this->_prepare_get_all_where($arr,$disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = &$list[$i];

			if(isset($r[$info['category']]) === false)
				$r[$info['category']] = array();

			$ref = &$r[$info['category']];
			$ref['id'] = $info['id'];
			$ref[$info['var_name']] = $info['var_val'];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_all_category($arr=array(),$disable=null)
	{
		if(($list = $this->_prepare_get_all_where($arr,$disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$list[$i]['category']] = $list[$i];

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_all($disable=null,$prepare=true)
	{
		$prepare = (bool) $prepare;

		if(($list = $this->_dso->get_all($disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $prepare === true ? $this->_prepare($list[$i]) : $list[$i];

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_where($arr=array(),$disable=null,$prepare=true)
	{
		$prepare = (bool) $prepare;

		if(($list = $this->_prepare_get_all_where($arr,$disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $prepare === true ? $this->_prepare($list[$i]) : $list[$i];

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function _prepare_get_all_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;

		unset($arr['commented']);

		$arr['filename'] = $this->_filename;

		if(($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get_all_where($where,$disable)) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_name($arr=array(),$disable=null)
	{
		if(($list = $this->_prepare_get_all_where($arr,$disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$list[$i]['var_name']] = $list[$i];

		if(empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
