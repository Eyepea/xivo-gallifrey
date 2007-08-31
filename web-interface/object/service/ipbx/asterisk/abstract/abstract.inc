<?php

class xivo_service_asterisk_abstract
{
	var $_conf	= array();
	var $_sre	= null;

	function get_name()
	{
		return($this->_name);
	}

	function _chk_exists_cfg($type,$name)
	{
		$r = false;

		$type = (string) $type;
		$name = (string) $name;

		if(xivo_issa($type,$this->_conf) === true && xivo_ak($name,$this->_conf[$type]) === true)
			$r = true;

		return($r);
	}

	function get_option()
	{
		if(xivo_issa('option',$this->_conf) === false)
			return(false);

		return($this->_conf['option']);
	}

	function get_option_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('option',$this->_conf) === false || isset($this->_conf['option'][$key]) === false)
			return(false);

		return($this->_conf['option'][$key]);
	}

	function get_element()
	{
		if(xivo_issa('element',$this->_conf) === false)
			return(false);

		return($this->_conf['element']);
	}

	function get_element_where($where)
	{
		$r = false;

		if(is_array($where) === false || xivo_issa('element',$this->_conf) === false)
			return($r);

		$arr_keys = array_keys($this->_conf['element']);

		if(($nb = count($arr_keys)) === 0)
			return($r);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$key = &$arr_keys[$i];

			if(array_key_exists($key,$where) === true && $this->unset_element_value($key) === false)
				$r[$key] = $where[$key];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_element_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['value']) === false)
			return(false);

		return($this->_conf['element'][$key]['value']);
	}

	function _set_element_default($key,$value)
	{
		$key = (string) $key;
		$value = (string) $value;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false)
			return(false);

		$this->_conf['element'][$key]['default'] = $value;

		return(true);
	}

	function get_element_default($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['default']) === false)
			return(false);

		return($this->_conf['element'][$key]['default']);
	}

	function isset_element_default($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| xivo_ak('setdef',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function unset_element_default($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === true && xivo_issa($key,$this->_conf['element']) === true
		&& isset($this->_conf['element'][$key]['setdef']) === true && $this->_conf['element'][$key]['setdef'] === false)
			return(true);

		return(false);
	}

	function isset_element_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| xivo_ak('set',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function unset_element_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['unset']) === false)
			return(false);

		return($this->_conf['element'][$key]['unset']);
	}

	function isnull_element_value($key)
	{
		$key = (string) $key;

		if(xivo_issa('element',$this->_conf) === false || xivo_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['null']) === false)
			return(false);

		return($this->_conf['element'][$key]['null']);
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.
		       XIVO_SEP_DIR.'ipbx'.
		       XIVO_SEP_DIR.'asterisk'.
		       XIVO_SEP_DIR.$this->_name.
		       XIVO_SEP_DIR.'config';

		if(($this->_conf = xivo_gat::get_file($dir)) === false)
			$this->_conf = array();
	}

	function _load_filter()
	{
		if(xivo::load_class('xivo_filter') === false || xivo_issa('filter',$this->_conf) === false)
			return(false);

		$this->_filter = new xivo_filter($this->_conf['filter']);

		return(true);
	}

	function add_origin()
	{
		return($this->_origin('add'));
	}

	function edit_origin()
	{
		return($this->_origin('edit'));
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true
		|| isset($this->_origin) === false
		|| is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,$this->_origin['id']);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function _prepare($data)
	{
		return($data);
	}

	function get($id,$disable=null,$delete=null)
	{
		if(($id = xivo_uint($id)) === 0
		|| ($r = $this->_dso->get($id,$disable,$delete)) === false)
			return(false);

		if(isset($this->_origin) === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_id($arr,$disable=null,$delete=null)
	{
		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_id($arr,$disable,$delete));
	}

	function get_where($arr,$disable=null,$delete=null)
	{
		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get($where,$disable,$delete)) === false)
			return(false);

		if(isset($this->_option) === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_nb($arr=null,$disable=null,$delete=null)
	{
		if($arr !== null && is_array($arr) === false)
			return(false);

		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_nb($arr,$disable,$delete));
	}

	function get_list($disable=null,$delete=null,$order=null,$limit=null)
	{
		if(($r = $this->_dso->get_list($disable,$delete,$order,$limit)) !== false
		&& isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$delete=null,$order=null,$limit=null)
	{
		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get_list_where($where,$disable,$delete,$order,$limit)) === false
		|| isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_all($disable=null,$delete=null,$prepare=true,$order=null,$limit=null)
	{
		$prepare = (bool) $prepare;

		if(($list = $this->_dso->get_all($disable,$delete,$order,$limit)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		if($prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $this->_prepare($list[$i]);
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $list[$i];
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_where($arr,$disable=null,$delete=null,$prepare=true,$order=null,$limit=null)
	{
		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable,$delete,$order,$limit)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		if($prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $this->_prepare($list[$i]);
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $list[$i];
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null,$delete=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = $this->_dso->edit($id,$arr,$disable,$delete);
		
		return($r);
	}

	function edit_where($id,$arr,$disable=null,$delete=null)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false
		|| ($arr = $this->chk_values($arr,true,false,false)) === false)
			return(false);

		$r = $this->_dso->edit($where,$arr,$disable,$delete);

		return($r);
	}

	function delete($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id,$disable));
	}

	function delete_where($id,$disable=null)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		return($this->_dso->delete($where,$disable));
	}

	function enable($id,$value=false)
	{
		$value = intval((bool) $value);

		return($this->edit($id,array('commented' => $value)));
	}

	function disable($id,$value=true)
	{
		$value = intval((bool) $value);

		return($this->edit($id,array('commented' => $value)));
	}

	function enable_where($id,$value=false)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		$value = intval((bool) $value);

		$r = $this->_dso->edit($where,array('commented' => $value));

		return($r);
	}

	function disable_where($id,$value=true)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		$value = intval((bool) $value);

		$r = $this->_dso->edit($where,array('commented' => $value));

		return($r);
	}

	function get_filter_err()
	{
		if($this->_filter === null)
			return(false);

		return($this->_filter->get_err());
	}

	function get_filter_result()
	{
		if($this->_filter === null)
			return(false);

		return($this->_filter->get_result());
	}

	function chk_values($arr,$set=true,$setval=true,$empty=true)
	{
		$set = (bool) $set;
		$setval = (bool) $setval;
		$empty = (bool) $empty;

		if($this->_filter === null && $this->_load_filter() === false)
			return(false);

		if($this->_filter->chk($arr,$setval) === false)
			return(false);

		$rs = $this->_filter->get_result();

		if($set === false || ($arr = xivo_get_aks($rs)) === false)
			return($rs);

		$r = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$rs[$key];

			if(($sval = $this->_set_value($key,$val)) !== false)
				$val = $sval;

			if($val === '' && $this->isnull_element_value($key) === true)
				$val = null;
			else if($val === null && $this->isnull_element_value($key) === false)
				$val = '';

			if($val === '' && $this->isset_element_default($key) === true
			&& ($dval = $this->get_element_default($key)) !== false)
				$val = $dval;

			if($this->unset_element_value($key) === true)
				continue;

			$r[$key] = $val;
		}

		if($empty === false && empty($r) === true)
			return(false);
			
		return($r);
	}

	function chk_value($key,$value)
	{
		if($this->_filter === null && $this->_load_filter() === false)
			return(false);

		if($this->_chk_exists_cfg('filter',$key) === false)
			return(null);

		return($this->_filter->chk_value($key,$value));
	}

	function _set_value($key,$val)
	{
		if($this->isset_element_value($key) === false)
			return($val);

		if(($value = $this->get_element_value($key)) === false)
			return(false);

		if(is_scalar($value) === true)
			return($value);

		if(xivo_ak($val,$value) === true)
			return($value[$val]);

		return(false);
	}

	function set_chk_value($key,$value)
	{
		$r = '';
		
		if(($val = $this->chk_value($key,$value)) !== false)
		{
			if(($sval = $this->_set_value($key,$val)) !== false)
				$r = $sval;
			else
				$r = $val;
		}

		if($r === '' && $this->isnull_element_value($key) === true)
			$r = null;
		else if($r === null && $this->isnull_element_value($key) === false)
			$r = '';

		if($r !== ''
		|| ($dval = $this->get_element_default($key)) === false
		|| $this->unset_element_default($key) === true)
			return($r);

		return($dval);
	}
}

?>
