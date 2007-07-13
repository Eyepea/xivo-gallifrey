<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','staticconf','staticconf.inc'),true);

class xivo_service_asterisk_generalsip extends xivo_service_asterisk_staticconf
{
	var $_dso		= null;
	var $_name		= 'generalsip';
	var $_filename		= 'sip.conf';
	var $_category		= 'general';
	var $_filter		= null;
	var $_generalsip	= array();

	function xivo_service_asterisk_generalsip(&$sre,&$dso)
	{
		$this->_load_staticconf_config();

		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_dso->set_param('filename',$this->_filename);

		$this->_load_config();
		$this->_generalsip['conf'] = $this->_conf;
	}

	function get_by_name($name,$disable=null)
	{
		return($this->get_where(array('var_name' => $name),$disable));
	}

	function get_where($arr,$disable=null)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;

		return(parent::get_where($arr,$disable));
	}

	function add($arr,$id=0)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;

		return(parent::add($arr,$id));
	}

	function edit($id,$arr)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;
		
		return(parent::edit($id,$arr));
	}

	function edit_where($arr,$val,$disable=null)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;

		$this->_conf = &$this->_staticconf['conf'];

		$r = parent::edit_where($arr,$val,$disable);

		$this->_conf = &$this->_generalsip['conf'];

		return($r);
	}

	function replace_val_list($arr,$disable=null)
	{
		$arr = (array) $arr;

		if(($list = xivo_get_aks($arr)) === false)
			return(false);

		for($i = 0;$i < $list['cnt'];$i++)
		{
			$name = &$list['keys'][$i];
			$val = &$arr[$name];

			$this->replace_val_by_name($name,$val,$disable);
		}

		return(true);
	}

	function replace_by_name($name,$arr)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;
		$arr['var_name'] = $name;

		return(parent::replace_by_name($name,$arr));
	}

	function replace_val_by_name($name,$val,$disable=null)
	{
		$disable = $disable === null ? $disable : intval((bool) $disable);

		$arr = array();
		$arr['var_val'] = $val;

		if($val !== null)
			$arr['commented'] = $disable !== null ? $disable : 0;
		else
			$arr['commented'] = $disable !== null ? $disable : 1;

		return($this->replace_by_name($name,$arr));
	}

	function get_list_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;

		$this->_conf = &$this->_staticconf['conf'];

		$r = parent::get_list_where($arr,$disable);

		$this->_conf = &$this->_generalsip['conf'];

		return($r);
	}

	function _prepare_get_all_where($arr=array(),$disable=null)
	{
		$this->_conf = &$this->_staticconf['conf'];

		$r = parent::_prepare_get_all_where($arr,$disable);

		$this->_conf = &$this->_generalsip['conf'];

		return($r);
	}

	function get_all_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;

		$this->_conf = &$this->_staticconf['conf'];

		$r = parent::get_all_where($arr,$disable);

		$this->_conf = &$this->_generalsip['conf'];

		return($r);
	}
}

?>
