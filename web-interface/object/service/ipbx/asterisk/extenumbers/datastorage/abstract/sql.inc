<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_extenumbers_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'extenumbers');

	function exists_exten($arr,$context='')
	{
		$context = (string) $context;

		if(($where = $this->_dso->where($arr)) === false)
			return(null);

		if($context !== '')
			$where .= ' AND (context = \''.$this->_dso->escape_string($context).'\' OR context = \'\')';

		if(($r = $this->_dso->select_one($this->_param['table'],'id',$where)) !== false)
			$r = (int) $r['id'];

		return($r);
	}

	function exists_type($arr,$context='')
	{
		$context = (string) $context;

		if(($where = $this->_dso->where($arr)) === false)
			return(null);

		if($context !== '')
			$where .= ' AND (context = \''.$this->_dso->escape_string($context).'\' OR context = \'\')';

		$r = $this->_dso->select_one($this->_param['table'],'type,typeval',$where);

		return($r);
	}

	function get_search($exten,$extenhash,$context='',$type='',$typeval='')
	{
		$where = array();

		if(($rs = $this->_dso->search_contain('exten',$exten)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_exact('extenhash',$extenhash)) !== false)
			$where[] = $rs;

		if(($where = implode(' OR ',$where)) === '')
			return(false);

		$whereand = array();

		if(xivo_haslen($context) !== false
		&& ($rs = $this->_dso->search_exact('context',$context)) !== false)
			$whereand[] = $rs;

		if(xivo_haslen($type) !== false
		&& ($rs = $this->_dso->search_exact('type',$type)) !== false)
			$whereand[] = $rs;

		if(xivo_haslen($typeval) !== false
		&& ($rs = $this->_dso->search_exact('typeval',$typeval)) !== false)
			$whereand[] = $rs;

		if(isset($whereand[0]) === true && ($whereand = implode(' AND ',$whereand)) !== '')
			$where = '('.$where.') AND '.$whereand;

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

		return($r);
	}
}

?>
