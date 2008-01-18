<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_groupfeatures_sql extends xivo_service_asterisk_abstract_sql
{
	var $_disable	= 'deleted';

	function get_last()
	{
		$this->_dso->new_select($this->_table,array('cnt' => (object) 'COUNT(*)',
							    'id' => (object) 'MAX(id)'));

		if(($r = $this->_dso->select_single()) !== false)
			$r = $r['cnt'] > 0 ? $r['id'] : 0;

		return($r);
	}

	function get_free()
	{
		$this->_dso->new_select($this->_table,'id');
		$this->_dso->where(array($this->_disable => 1));
		$this->_dso->new_order(array('id' => SORT_ASC));

		$r = $this->_dso->select_one();

		$this->_dso->reset_order();

		return($r);
	}

	function delete($arr)
	{
		return(parent::delete($arr,null));
	}
}

?>
