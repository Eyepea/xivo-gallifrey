<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_extenumbers_sql extends xivo_service_asterisk_abstract_sql
{
	var $_disable	= null;

	function exists_type($arr,$context='')
	{
		if(is_array($arr) === false)
			return(false);

		$context = (string) $context;

		$this->_dso->new_select($this->_table,array('type','typeval'));
		$this->_dso->where($arr);

		if($context !== '')
			$this->_dso->andwhere('(context = %s OR context = \'\')',$context);

		return($this->_dso->select_single());
	}

	function search($exten,$extenhash,$context='',$type='',$typeval='')
	{
		$this->_cnt = 0;

		$this->_dso->new_select($this->_table);
		$this->_dso->wherecontain('exten',$exten);
		$this->_dso->orwhere(array('extenhash' => $extenhash));

		if($this->_dso->aggregate_where() === false)
			return(false);

		$where = array();

		if(xivo_haslen($context) === true)
			$where['context'] = $context;

		if(xivo_haslen($type) === true)
			$where['type'] = $type;

		if(xivo_haslen($typeval) === true)
			$where['typeval'] = $typeval;

		if(empty($where) === false)
			$this->_dso->andwhere($where);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count()) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
