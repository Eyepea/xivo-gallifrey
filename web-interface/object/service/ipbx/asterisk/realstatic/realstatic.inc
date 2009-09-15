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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_realstatic extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'realstatic';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;
	var $_filename		= false;
	var $_cat_metric	= null;
	var $_var_metric	= null;
	var $_category		= null;
	var $_var_name		= null;

	function xivo_service_asterisk_realstatic(&$sre,&$dso,$param=array())
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
		$this->load_init($param);
	}

	function load_init($param=array())
	{
		$vars = get_class_vars(get_class($this));

		$this->_cat_metric = $vars['_cat_metric'];
		$this->_var_metric = $vars['_var_metric'];
		$this->_category = $vars['_category'];
		$this->_var_name = $vars['_var_name'];

		if(is_array($param) === true && empty($param) === false)
		{
			if(isset($param['filename']) === true)
				$this->_filename = $param['filename'];

			if(isset($param['cat_metric']) === true)
				$this->_cat_metric = (int) $param['cat_metric'];

			if(isset($param['var_metric']) === true)
				$this->_var_metric = (int) $param['var_metric'];

			if(isset($param['category']) === true)
				$this->_category = strval($param['category']);

			if(isset($param['var_name']) === true)
				$this->_var_name = strval($param['var_name']);
		}

		if($this->chk_value('filename',$this->_filename) === false)
			trigger_error('Invalid filename parameter in '.__CLASS__,E_USER_ERROR);

		if(isset($this->_category) === true
		&& ($this->_category = $this->chk_value('category',$this->_category)) === false)
			trigger_error('Invalid category parameter in '.__CLASS__,E_USER_ERROR);

		if(isset($this->_var_name) === true
		&& ($this->_var_name = $this->chk_value('var_name',$this->_var_name)) === false)
			trigger_error('Invalid var_name parameter in '.__CLASS__,E_USER_ERROR);

		$this->_dso->set_param('filename',$this->_filename);
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function get_nb($arr=array(),$disable=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$arr['filename'] = $this->_filename;

		if(isset($this->_cat_metric) === true)
			$arr['cat_metric'] = $this->_cat_metric;

		if(isset($this->_var_metric) === true)
			$arr['var_metric'] = $this->_var_metric;

		if(isset($this->_category) === true)
			$arr['category'] = $this->_category;

		if(isset($this->_var_name) === true)
			$arr['var_name'] = $this->_var_name;

		return(parent::get_nb($arr,$disable));
	}

	function get_all($disable=null,$prepare=true,$order=null,$limit=null)
	{
		return($this->_prepare_get_all_where(null,$disable,$prepare,$order,$limit));
	}

	function get_all_where($arr,$disable=null,$prepare=true,$order=null,$limit=null)
	{
		return($this->_prepare_get_all_where($arr,$disable,$prepare,$order,$limit));
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

		if(($id = xivo_ulongint($id)) !== 0)
			$r['id'] = $id;

		$r['filename'] = $this->_filename;

		if(isset($this->_cat_metric) === true)
			$arr['cat_metric'] = $this->_cat_metric;

		if(isset($this->_var_metric) === true)
			$arr['var_metric'] = $this->_var_metric;

		if(isset($this->_category) === true)
			$arr['category'] = $this->_category;

		if(isset($arr['var_name']) === false && isset($this->_var_name) === true)
			$arr['var_name'] = $this->_var_name;

		if(isset($arr['cat_metric']) === true)
			$r['cat_metric'] = (int) $arr['cat_metric'];

		if(isset($arr['var_metric']) === true)
			$r['var_metric'] = (int) $arr['var_metric'];

		$r['category'] = $arr['category'];
		$r['var_name'] = $arr['var_name'];
		$r['var_val'] = $arr['var_val'];

		if($arr['var_val'] === null)
			$r['commented'] = true;
		else if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];
		else
			$r['commented'] = false;

		return($this->_dso->add($r));
	}

	function add_category($category,$arr)
	{
		if(is_array($arr) === false)
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
		if(($id = xivo_ulongint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = array();

		$r['filename'] = $this->_filename;

		if(isset($this->_cat_metric) === true)
			$arr['cat_metric'] = $this->_cat_metric;

		if(isset($this->_var_metric) === true)
			$arr['var_metric'] = $this->_var_metric;

		if(isset($this->_category) === true)
			$arr['category'] = $this->_category;

		if(isset($arr['var_name']) === false && isset($this->_var_name) === true)
			$arr['var_name'] = $this->_var_name;

		if(isset($arr['cat_metric']) === true)
			$r['cat_metric'] = (int) $arr['cat_metric'];

		if(isset($arr['var_metric']) === true)
			$r['var_metric'] = (int) $arr['var_metric'];

		if(isset($arr['category']) === true)
			$r['category'] = $arr['category'];

		if(isset($arr['var_name']) === true)
			$r['var_name'] = $arr['var_name'];

		if(array_key_exists('var_val',$arr) === true)
			$r['var_val'] = $arr['var_val'];

		if(array_key_exists('var_val',$arr) === true && $arr['var_val'] === null)
			$r['commented'] = true;
		else if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];

		return($this->_dso->edit($id,$r));
	}

	function edit_where($where,$arr,$disable=null,$id=null,$chkunique=true)
	{
		if(is_array($arr) === false)
			return(false);

		$var_val_exists = array_key_exists('var_val',$arr);

		if($var_val_exists === true && $arr['var_val'] === null)
			$arr['commented'] = true;
		else if(isset($arr['commented']) === true)
			$arr['commented'] = (bool) $arr['commented'];
		else if($var_val_exists === true && xivo_has_len($arr['var_val']) === true)
			$arr['commented'] = false;
		else if($var_val_exists === true && $arr['var_val'] === '')
			$arr['commented'] = true;
		else
			unset($arr['commented']);

		return(parent::edit_where($where,$arr,$disable,$id,$chkunique));
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

	function replace_by_name($name,$arr)
	{
		if(isset($arr['var_name']) === false)
			$arr['var_name'] = $name;

		if(($rs = $this->get_where(array('var_name' => $name))) === false)
			return($this->add($arr));

		return($this->edit($rs['id'],$arr));
	}

	function replace_val_by_name($name,$val)
	{
		return($this->replace_by_name($name,array('var_val' => $val)));
	}

	function delete_by_category($category)
	{
		return(parent::delete_where(array('category' => $category)));
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

	function get_all_by_category($arr=array(),$disable=null)
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

			$r[$info['category']][$info['var_name']] = $info;
		}

		if(isset($this->_category,$r[$this->_category]) === true)
			$r = $r[$this->_category];

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

	function get_all_val_by_category($category='',$disable=null)
	{
		if(xivo_has_len($category) === false)
		{
			if(isset($this->_category) === true)
				$category = $this->_category;
			else
				return(false);
		}

		if(($list = $this->_prepare_get_all_where(array('category' => $category),
							  $disable,
							  true,
							  array('id' => SORT_ASC))) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = &$list[$i];

			if(isset($r[$info['var_name']]) === false)
				$r[$info['var_name']] = $info;
			else
			{
				if(isset($r[$info['var_name']]['id']) === true)
				{
					$preinfo = $r[$info['var_name']];
					$r[$info['var_name']] = array();
					$r[$info['var_name']][] = $preinfo;
				}

				$r[$info['var_name']][] = $info;
			}
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function _prepare_get_all_where($arr=array(),$disable=null,$prepare=true,$order=null,$limit=null)
	{
		$arr = (array) $arr;

		unset($arr['commented']);

		$arr['filename'] = $this->_filename;

		if(isset($this->_cat_metric) === true)
			$arr['cat_metric'] = $this->_cat_metric;

		if(isset($this->_var_metric) === true)
			$arr['var_metric'] = $this->_var_metric;

		if(isset($this->_category) === true)
			$arr['category'] = $this->_category;

		if(isset($this->_var_name) === true)
			$arr['var_name'] = $this->_var_name;

		return(parent::get_all_where($arr,$disable,$prepare,$order,$limit));
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
