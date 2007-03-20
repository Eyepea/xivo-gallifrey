<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_staticconf extends xivo_service_asterisk_abstract
{
	function get($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$r['commented'] = (bool) $r['commented'];

		return($r);
	}

	function get_where($arr,$disable=null)
	{
		$arr = (array) $arr;

		$where = array();
		$where['filename'] = $this->_filename;

		if(isset($arr['category']) === true)
			$where['category'] = $arr['category'];

		if(isset($arr['var_name']) === true)
			$where['var_name'] = $arr['var_name'];

		if(isset($arr['var_val']) === true)
			$where['var_val'] = $arr['var_val'];

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

		if(($id = xivo_uint($id)) !== 0)
			$r['id'] = $id;

		$r['filename'] = $this->_filename;
		$r['category'] = $arr['category'];
		$r['var_name'] = $arr['var_name'];
		$r['var_val'] = $arr['var_val'];

		if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];
		else if($this->disable_element_value($arr['var_name']) === true
		&& ($arr['var_val'] === '' || $arr['var_val'] === null) === true)
			$r['commented'] = true;
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
		$r['category'] = $arr['category'];
		$r['var_name'] = $arr['var_name'];
		$r['var_val'] = $arr['var_val'];

		if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];
		else if($this->disable_element_value($arr['var_name']) === true
		&& ($arr['var_val'] === '' || $arr['var_val'] === null) === true)
			$r['commented'] = true;
		else
			$r['commented'] = false;

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
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
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

		$where = array();
		$where['filename'] = $this->_filename;

		if(isset($arr['category']) === true)
			$where['category'] = $arr['category'];

		if(isset($arr['var_name']) === true)
			$where['var_name'] = $arr['var_name'];

		if(isset($arr['var_val']) === true)
			$where['var_val'] = $arr['var_val'];

		if(($r = $this->_dso->get_list_where($where,$disable)) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_name_val($arr=array(),$disable=null)
	{
		$r = false;

		$arr = (array) $arr;

		if(($list = $this->get_list_where($arr)) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

			$r[$info['var_name']] = $info['var_val'];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_name_val_by_category($arr=array(),$disable=null)
	{
		$r = false;

		if(($list = $this->get_list_where($arr)) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

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
		$r = false;

		if(($list = $this->get_list_where($arr)) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

			$r[$info['category']] = $info;
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_all($disable=null)
	{
		$r = false;

		if(($list = $this->get_list($disable)) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_where($arr=array(),$disable=null)
	{
		$r = false;

		if(($list = $this->get_list_where($arr)) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_name($arr=array(),$disable=null)
	{
		$r = false;

		if(($list = $this->get_list_where($arr)) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

			$r[$info['var_name']] = $info;
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
