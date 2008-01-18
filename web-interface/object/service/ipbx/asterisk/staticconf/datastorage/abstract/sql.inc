<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_staticconf_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	function get($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_table);

		if(isset($this->_forcecolumn) === true)
			$arr = $this->_forcecolumn;
		else
			$arr = array();

		$arr['id'] = $id;

		if($disable !== null)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		return($this->_dso->select_single());
	}

	function get_where($arr,$disable=null)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		$this->_dso->new_select($this->_table,'id');

		if($disable !== null)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		return($this->_dso->select_one());
	}

	function add($arr)
	{
		$r = false;

		if($this->_dso->insert($this->_table,$arr) === true)
			$r = $this->_dso->insert_id();

		return($r);
	}

	function edit($id,$value)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->update($this->_table,$value,array('id' => $id)));
	}

	function edit_where($arr,$value,$disable=null)
	{
		if(is_array($arr) === false || is_array($value) === false)
			return(false);

		if($disable !== null)
			$arr[$this->_disable] = intval((bool) $disable);

		return($this->_dso->update($this->_table,$value,$arr));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($this->_table,array('id' => $id)));
	}

	function delete_where($arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if($disable !== null)
			$arr[$this->_disable] = intval((bool) $disable);

		return($this->_dso->delete($this->_table,$arr));
	}
}

?>
