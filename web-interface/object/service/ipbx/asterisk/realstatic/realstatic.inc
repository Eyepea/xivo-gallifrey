<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_realstatic extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'realstatic';
	var $_filter	= false;
	var $_origin	= false;
	var $_filename	= '';

	function xivo_service_asterisk_realstatic(&$sre,&$dso,$param)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		if(is_array($param) === false)
			trigger_error('Invalid parameter in '.__CLASS__,E_USER_ERROR);

		if(isset($param['filename']) === false
		|| preg_match('/^[a-zA-Z0-9_-]+\.conf$/',$param['filename']) === 0)
			trigger_error('Invalid filename parameter in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;
		$this->_filename = $param['filename'];

		$this->_dso->set_param('filename',$this->_filename);

		$this->_load_config();
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];

		return($data);
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

		if(($id = xivo_uint($id)) !== 0)
			$r['id'] = $id;

		$r['filename'] = $this->_filename;
		$r['category'] = $arr['category'];
		$r['var_name'] = $arr['var_name'];
		$r['var_val'] = $arr['var_val'];

		if(isset($arr['cat_metric']) === true)
			$r['cat_metric'] = (int) $arr['cat_metric'];

		if(isset($arr['var_metric']) === true)
			$r['var_metric'] = (int) $arr['var_metric'];

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

		if(($a = xivo_get_aks($arr)) === false)
			return(false);

		for($i = 0;$i < $a['cnt'];$i++)
		{
			$key = &$a['keys'][$i];
			$add['var_name'] = $key;
			$add['var_val'] = &$arr[$key];

			if(($id = $this->add($add)) === false)
				break;

			$r[$i] = $id;
		}

		if(isset($r[0]) === false)
			return(false);

		if($id !== false)
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
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
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

		if(array_key_exists('var_val',$arr) === true && $arr['var_val'] === null)
			$r['commented'] = true;
		else if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];

		return($this->_dso->edit($id,$r));
	}

	function edit_where($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if(array_key_exists('var_val',$arr) === true && $arr['var_val'] === null)
			$arr['commented'] = true;
		else if(isset($arr['commented']) === true)
			$arr['commented'] = (bool) $arr['commented'];
		else
			unset($arr['commented']);

		return(parent::edit_where($id,$arr,$disable,null));
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

		if(($a = xivo_get_aks($arr)) === false)
			return(false);

		for($i = 0;$i < $a['cnt'];$i++)
		{
			$key = &$a['keys'][$i];

			$where['var_name'] = $key;

			$edit['var_name'] = $key;
			$edit['var_val'] = &$arr[$key];

			if(($info = $this->get_where($where)) === false)
				continue;

			if(($ret = $this->edit($info['id'],$edit)) === false)
				break;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			return(false);

		if($ret !== false)
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

	function delete_by_category($category)
	{
		return($this->_dso->delete_where(array('category' => $category)));
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

		if(isset($arr['category'],$r[$arr['category']]) === true)
			$r = $r[$arr['category']];

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
