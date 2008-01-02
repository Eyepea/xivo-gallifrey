<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libdatastorage.inc');

class xivo_service_asterisk_abstract extends xivo_libdatastorage_abstract
{
	var $_conf	= array();
	var $_sre	= null;

	function get_name()
	{
		return($this->_name);
	}

	function &get_dso()
	{
		$r = false;

		if(isset($this->_dso) === true && is_object($this->_dso) === true)
			$r = &$this->_dso;

		return(($ref = &$r));
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

	function enable($id,$value=false)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array('commented' => $value)));
	}

	function disable($id,$value=true)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array('commented' => $value)));
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

	function chk_dest_endcall($val)
	{
		return($this->_chk_dest_value('endcall',$val));
	}

	function chk_dest_application($val,$appval)
	{
		if($this->_chk_dest_value('application',$val) === false
		|| xivo::load_class('xivo_filter') === false
		|| ($filters = $this->_chk_exists_cfg('applicationval','filter',true)) === false)
			return(false);

		$filter = new xivo_filter($filters);

		return($filter->chk_value($val,$appval));
	}

	function _chk_dest_value($type,$val)
	{
		if(is_scalar($val) === false
		|| ($value = $this->get_element_value($type)) === false
		|| in_array($val,$value,true) === false)
			return(false);

		return(true);
	}

	function chk_dest_sound($dir,$val)
	{
		if(($sounds = $this->_sre->get_module('sounds')) === false
		|| ($list = $sounds->get_list($dir,true)) === false
		|| isset($list[$val]) === false)
			return(false);

		return($val);
	}

	function chk_dest_custom($val)
	{
		if(preg_match('/^[a-zA-Z]{3,}\((.*)\)$/',$val,$match) !== 1)
			return(false);

		if($match[1] === '' || ctype_print($match[1]) === true)
			return(true);

		return(false);
	}

	function chk_dest_schedule($val,$id=null,$disable=null)
	{
		return($this->_chk_dest('schedule',$val,$id,$disable,false));
	}

	function chk_dest_user($val,$id=null,$disable=null,$number=true)
	{
		return($this->_chk_dest('userfeatures',$val,$id,$disable,$number));	
	}

	function chk_dest_group($val,$id=null,$disable=null,$number=true)
	{
		return($this->_chk_dest_group_queue('group',$val,$id,$disable,$number));
	}

	function chk_dest_queue($val,$id=null,$disable=null,$number=true)
	{
		return($this->_chk_dest_group_queue('queue',$val,$id,$disable,$number));
	}

	function _chk_dest_group_queue($type,$val,$id=null,$disable=null,$number=true)
	{
		$val = intval($val);

		if($type !== 'group' && $type !== 'queue')
			return(false);

		if(($info = $this->_chk_dest($type.'features',$val,$id,null,$number)) === false
		|| $disable === null)
			return($info);

		$module = $this->_sre->get_module($type.'features');

		if(($info = $module->get($val)) === false
		|| ($queue = $this->_sre->get_module('queue')) === false
		|| ($queue->get($info['name'],$disable)) === false)
			return(false);

		return($info);
	}

	function chk_dest_meetme($val,$id=null,$disable=null,$number=true)
	{
		return($this->_chk_dest('meetmefeatures',$val,$id,$disable,$number));
	}

	function _chk_dest($type,$val,$id=null,$disable=null,$number=true)
	{
		$val = intval($val);
		$number = (bool) $number;

		if($id !== null)
			$id = xivo_uint($id);

		if(($module = $this->_sre->get_module($type)) === false
		|| ($info = $module->get($val,$disable)) === false
		|| ($id !== null && $id === $info['id']) === true
		|| ($number === true && xivo_haslen($info['number']) === false) === true)
			return(false);

		return($info);
	}
}

?>
