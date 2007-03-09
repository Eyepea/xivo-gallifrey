<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'staticconf'.XIVO_SEP_DIR.'staticconf.inc');

class xivo_service_asterisk_meetme extends xivo_service_asterisk_staticconf
{
	var $_dso	= null;
	var $_name	= 'meetme';
	var $_filename	= 'meetme.conf';
	var $_origin	= null;

	function xivo_service_asterisk_meetme(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;
	}

	function get($id,$disable=null)
	{
		if(($r = parent::get($id,$disable)) === false)
			return($r);

		$this->_origin = $r;

		$val = explode(',',$r['var_val']);

		$r['number'] = isset($val[0]) === true ? $val[0] : '';
		$r['pin'] = isset($val[1]) === true ? $val[1] : '';
		$r['admpin'] = isset($val[2]) === true ? $val[2] : '';

		return($r);
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
		if(($type === 'add' && $type === 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,$this->_origin['id']);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function add($arr,$id=0)
	{
		$arr = (array) $arr;

		$arr['category'] = 'rooms';
		$arr['var_name'] = 'conf';
		$arr['var_val'] = xivo_digit($arr['number'],0).',';

		if(isset($arr['pin']) === true && ($pin = xivo_digit($arr['pin'])) !== false)
			$arr['var_val'] .= $pin;

		if(isset($arr['admpin']) === true && ($admpin = xivo_digit($arr['admpin'])) !== false)
			$arr['var_val'] .= ','.$admpin;

		$arr['var_val'] = rtrim($arr['var_val'],',');

		return(parent::add($arr,$id));
	}

	function edit($id,$arr)
	{
		$arr = (array) $arr;

		$arr['category'] = 'rooms';
		$arr['var_name'] = 'conf';
		$arr['var_val'] = xivo_digit($arr['number'],0).',';

		if(isset($arr['pin']) === true && ($pin = xivo_digit($arr['pin'])) !== false)
			$arr['var_val'] .= $pin;

		if(isset($arr['admpin']) === true && ($admpin = xivo_digit($arr['admpin'])) !== false)
			$arr['var_val'] .= ','.$admpin;

		$arr['var_val'] = rtrim($arr['var_val'],',');

		return(parent::edit($id,$arr));
	}
}

?>
