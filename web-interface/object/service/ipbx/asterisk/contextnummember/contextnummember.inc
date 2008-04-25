<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_contextnummember extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'contextnummember';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_contextnummember(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['context']) === true)
			$arr['context'] = strtolower($arr['context']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['context']) === true)
			$arr['context'] = strtolower($arr['context']);

		return(parent::edit($id,$arr));
	}

	function get_all_by_type($arr,$disable=null,$prepare=true,$order=null,$limit=null)
	{
		$prepare = (bool) $prepare;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(is_array($arr) === false || empty($arr) === true)
			$list = $this->_dso->get_all($disable,$order,$limit);
		else if(($where = $this->get_element_where($arr)) !== false)
			$list = $this->_dso->get_all_where($where,$disable,$order,$limit);
		else
			return(false);

		if($list === false || isset($list[0]) === false)
			return(false);
		else if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		$r = array();

		if(is_array($list) === false || ($nb = count($list)) === 0)
			return(false);
		else if($prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				if(isset($r[$list[$i]['type']]) === false)
					$r[$list[$i]['type']] = array();

				$r[$list[$i]['type']][] = $this->_prepare($list[$i]);
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
			{
				if(isset($r[$list[$i]['type']]) === false)
					$r[$list[$i]['type']] = array();

				$r[$list[$i]['type']][] = $list[$i];
			}
		}

		return($r);
	}
}

?>
