<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'staticconf'.XIVO_SEP_DIR.'staticconf.inc');

class xivo_service_asterisk_generalsip extends xivo_service_asterisk_staticconf
{
	var $_dso	= null;
	var $_name	= 'generalsip';
	var $_filename	= 'sip.conf';
	var $_category	= 'general';
	var $_filter	= null;

	function xivo_service_asterisk_generalsip(&$sre,&$dso)
	{

		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get_by_name($name,$disable=null)
	{
		return(parent::get_where(array('var_name' => $name),$disable));
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

		if($val !== '' && $val !== null)
			$arr['commented'] = $disable !== null ? $disable : 0;
		else
			$arr['commented'] = $disable !== null ? $disable : 1;

		return($this->replace_by_name($name,$arr));
	}

	function get_list_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;

		return(parent::get_list_where($arr,$disable));
	}
}

?>
