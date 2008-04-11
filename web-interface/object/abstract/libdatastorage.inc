<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libconfig.inc');

class xivo_libdatastorage_abstract extends xivo_libconfig_abstract
{
	function set_dso_param($name,$value)
	{
		if(is_object($this->_dso) === false)
			return(false);

		return($this->_dso->set_param($name,$value));
	}

	function get_dso_param($name,$value)
	{
		if(is_object($this->_dso) === false)
			return(false);

		return($this->_dso->get_param($name,$value));
	}

	function _prepare(&$data)
	{
		return($data);
	}

	function _chk_primary_key($id)
	{
		if($this->_dso->is_single_primary() === true)
		{
			if(xivo_haslen($id) === false)
				return(false);
			else
				return($this->_dso->mk_primary_key_value($id));
		}
		else if($this->_dso->is_multi_primary() === false)
			return(false);
		else if(($primary = $this->_dso->get_primary_key()) !== false
		&& ($id = xivo_array_intersect_key($primary,$id)) !== false
		&& count($primary) === count($id))
			return($id);

		return(false);
	}

	function set_origin($arr)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;
		else
			return(false);

		if(is_array($arr) === false)
			return(false);

		$this->_origin = $arr;

		return(true);
	}

	function get_origin()
	{
		if(isset($this->_origin) === false
		|| is_array($this->_origin) === false)
			return(false);

		return($this->_origin);
	}

	function get_origin_list()
	{
		if(isset($this->_origin_list) === false
		|| is_array($this->_origin_list) === false)
			return(false);

		return($this->_origin_list);
	}

	function add_origin($val=null)
	{
		return($this->_origin('add',$val));
	}

	function add_origin_list($val=null)
	{
		return($this->_origin_list('add',$val));
	}

	function edit_origin($val=null)
	{
		return($this->_origin('edit',$val));
	}

	function edit_origin_list($val=null)
	{
		return($this->_origin_list('edit',$val));
	}

	function _origin_get_primary_key($type,$orig)
	{
		if($this->_dso->is_single_primary() === true)
		{
			if(($primary = $this->_dso->get_primary_key()) === false)
				return(false);

			$key = key($primary);

			if(isset($orig[$key]) === true)
				return($orig[$key]);
			
			return(false);
		}
		else if($this->_dso->is_multi_primary() === true && $type !== 'add')
		{
			if(($primary = $this->_dso->get_primary_key()) !== false
			&& ($origid = xivo_array_intersect_key($primary,$orig)) !== false
			&& count($primary) === count($origid))
				return($origid);

			return(false);
		}

		return(null);
	}

	function _origin($type,$val=null)
	{
		if(($type !== 'add' && $type !== 'edit') === true
		|| isset($this->_origin) === false)
			return(false);
		else if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(is_array($orig) === false
		|| ($origid = $this->_origin_get_primary_key($type,$orig)) === false)
			return(false);
		else if($type === 'add')
		{
			if($origid !== null)
				return($this->add($orig,$origid));

			return($this->add($orig));
		}
		else if($origid !== null)
			return($this->edit($origid,$orig));

		return($this->edit($orig));
	}

	function _origin_list($type,$val=null)
	{
		if(($type !== 'add' && $type !== 'edit') === true
		|| isset($this->_origin_list) === false)
			return(false);
		else if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin_list;

		if(is_array($orig) === false || ($nb = count($orig)) === 0)
			return(false);
		else if($type === 'add')
		{
			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$orig[$i];

				if(($origid = $this->_origin_get_primary_key($type,$ref)) === false)
					return(false);
				else if($origid !== null)
					$this->add($ref,$origid);
				else
					$this->add($ref);
			}

			return(true);
		}

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$orig[$i];

			if(($origid = $this->_origin_get_primary_key($type,$ref)) === false)
				return(false);
			else if($origid !== null)
				$this->edit($origid,$ref);
			else
				$this->edit($ref);
		}

		return(true);
	}

	function get($id,$disable=null)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(($id = $this->_chk_primary_key($id)) === false
		|| ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		if(isset($this->_origin) === true)
			$this->_origin = $r;

		return($this->_prepare($r));
	}

	function get_primary($arr,$disable=null,$id=null)
	{
		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);
		else if($id !== null)
		{
			if(is_array($arr) === false
			|| ($id = $this->_chk_primary_key($id)) === false)
				return(false);

			$arr = array_merge($arr,$id);
		}

		return($this->_dso->get_primary($arr,$disable));
	}

	function get_where($arr,$disable=null,$id=null)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false)
			return(false);
		else if($id !== null)
		{
			if(($id = $this->_chk_primary_key($id)) === false)
				return(false);

			$where = array_merge($where,$id);
		}

		if(($r = $this->_dso->get($where,$disable)) === false)
			return(false);
		else if(isset($this->_origin) === true)
			$this->_origin = $r;

		return($this->_prepare($r));
	}

	function get_nb($arr=null,$disable=null)
	{
		if(($arr !== null && is_array($arr) === false) === true
		|| (is_array($arr) === true
		   && ($arr = $this->get_element_where($arr)) === false) === true)
			return(false);

		return($this->_dso->get_nb($arr,$disable));
	}

	function get_cnt()
	{
		return($this->_dso->get_cnt());
	}

	function get_list($disable=null,$order=null,$limit=null)
	{
		if(($r = $this->_dso->get_list($disable,$order,$limit)) !== false
		&& isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null)
	{
		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get_list_where($where,$disable,$order,$limit)) === false
		|| isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_all($disable=null,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(($list = $this->_dso->get_all($disable,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);
		else if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);

		return($this->_mk_list_assoc_prepare($list,$assoc,$prepare));
	}

	function get_all_where($arr,$disable=null,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);
		else if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);

		return($this->_mk_list_assoc_prepare($list,$assoc,$prepare));
	}

	function _mk_list_assoc_prepare(&$list,$assoc=false,$prepare=false)
	{
		$assoc = (bool) $assoc;
		$prepare = (bool) $prepare;

		if($assoc === false && $prepare === false)
			return($list);
		else if(is_array($list) === false || ($nb = count($list)) === 0)
			return(false);

		$r = array();

		if($assoc === true && ($primary = $this->_dso->get_primary_key()) !== false)
		{
			if($this->_dso->is_single_primary() === true)
			{
				$key = key($primary);

				if($prepare === true)
				{
					for($i = 0;$i < $nb;$i++)
						$r[$list[$i][$key]] = $this->_prepare($list[$i]);
				}
				else
				{
					for($i = 0;$i < $nb;$i++)
						$r[$list[$i][$key]] = $list[$i];
				}
			}
			else if($prepare === true)
			{
				for($i = 0;$i < $nb;$i++)
				{
					if(($intersect = xivo_array_intersect_key($primary,$list[$i])) !== false
					&& ($key = implode('-',$intersect)) !== '')
						$r[$key] = $this->_prepare($list[$i]);
				}
			}
			else
			{
				for($i = 0;$i < $nb;$i++)
				{
					if(($intersect = xivo_array_intersect_key($primary,$list[$i])) !== false
					&& ($key = implode('-',$intersect)) !== '')
						$r[$key] = $list[$i];
				}
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $this->_prepare($list[$i]);
		}

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(empty($id) === false)
		{
			if(($id = $this->_chk_primary_key($id)) === false)
				return(false);

			$arr = array_merge($arr,$id);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false
		|| ($id = $this->_chk_primary_key($id)) === false)
			return(false);

		return($this->_dso->edit($id,$arr,$disable));
	}

	function edit_where($where,$arr,$disable=null,$id=null)
	{
		if(is_array($where) === false
		|| ($where = $this->get_element_where($where)) === false
		|| ($arr = $this->chk_values($arr,true,false,false)) === false)
			return(false);
		else if($id !== null)
		{
			if(($id = $this->_chk_primary_key($id)) === false)
				return(false);

			$where = array_merge($where,$id);
		}

		return($this->_dso->edit($where,$arr,$disable));
	}

	function delete($id,$disable=null)
	{
		if(($id = $this->_chk_primary_key($id)) === false)
			return(false);

		return($this->_dso->delete($id,$disable));
	}

	function delete_where($where,$disable=null,$id=null)
	{
		if(is_array($where) === false
		|| ($where = $this->get_element_where($where)) === false)
			return(false);
		else if($id !== null)
		{
			if(($id = $this->_chk_primary_key($id)) === false)
				return(false);

			$where = array_merge($where,$id);
		}

		return($this->_dso->delete($where,$disable));
	}

	function enable($id,$value=false)
	{
		if(($id = $this->_chk_primary_key($id)) === false
		|| ($disable = $this->_dso->get_disable()) === false)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array($disable => $value)));
	}

	function disable($id,$value=true)
	{
		if(($id = $this->_chk_primary_key($id)) === false
		|| ($disable = $this->_dso->get_disable()) === false)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array($disable => $value)));
	}

	function enable_where($id,$value=false)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false
		|| ($disable = $this->_dso->get_disable()) === false)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($where,array($disable => $value)));
	}

	function disable_where($id,$value=true)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false
		|| ($disable = $this->_dso->get_disable()) === false)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($where,array($disable => $value)));
	}
}

?>
