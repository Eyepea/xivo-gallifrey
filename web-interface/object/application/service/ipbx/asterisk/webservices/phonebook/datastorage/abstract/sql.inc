<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_webservices_phonebook_abstract_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'phonebook'		=> '',
				'phonebooknumber'	=> '');

	function search($value,$order=null,$limit=null)
	{
		$t = &$this->_table;
		$search = array();

		if(($rs = $this->_dso->search_contain('number',$value,$t['phonebooknumber'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('firstname',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('lastname',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('displayname',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($search = implode(' OR ',$search)) === '')
			return(false);

		$where = '('.$search.') AND '.$t['phonebooknumber'].'.type != \'fax\'';

		$phonebookid = $t['phonebook'].'.id';

		if(($phonebooknumberjoin = $this->_dso->join($t['phonebooknumber'],
							      'INNER',
							      $t['phonebooknumber'].'.phonebookid = '.$phonebookid)) === false)
			return(false);

		$select = $t['phonebook'].'.displayname AS name,'.
			  $t['phonebooknumber'].'.number AS phone,'.
			  $t['phonebooknumber'].'.type'; 
		$from = $t['phonebook'].' '.$phonebooknumberjoin;

		$this->_dso->new_group('id',$t['phonebooknumber']);
		$this->_dso->new_order($order,null,$t['phonebook']);
		$this->_dso->new_limit($limit);
	
		$r = $this->_dso->select_all($from,$select,$where);

		$this->_dso->reset_group();
		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count($from,$t['phonebook'].'.id',$where)) === false
		|| empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
