<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','datastorage.inc'),true);

class xivo_service_asterisk_abstract_sql extends xivo_service_asterisk_abstract_datastorage
{
	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso);

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function get_all($disable=null,$delete=null,$order=null,$limit=null)
	{
		$where = '1';

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		if($delete !== null)
			$where .= ' AND deleted = '.intval((bool) $delete);

		if($order !== null)
			$this->_dso->order($order);

		if($limit !== null)
			$this->_dso->limit($limit);

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

		if($order !== null)
			$this->_dso->order();

		if($limit !== null)
			$this->_dso->limit();

		return($r);
	}
}

?>
