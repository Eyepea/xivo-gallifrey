<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_phonebook_abstract_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'phonebook'		=> '',
				'phonebookaddress'	=> '',
				'phonebooknumber'	=> '');

	function search($value,$order=null,$limit=null)
	{
		$t = &$this->_table;
		$search = array();

		if(($rs = $this->_dso->search_contain('address1',$value,$t['phonebookaddress'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('address2',$value,$t['phonebookaddress'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('city',$value,$t['phonebookaddress'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('state',$value,$t['phonebookaddress'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('zipcode',$value,$t['phonebookaddress'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('country',$value,$t['phonebookaddress'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('number',$value,$t['phonebooknumber'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('firstname',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('lastname',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('displayname',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('society',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('email',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('url',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($rs = $this->_dso->search_contain('description',$value,$t['phonebook'])) !== false)
			$search[] = $rs;

		if(($search = implode(' OR ',$search)) === '')
			return(false);

		$where = '('.$search.')';

		$phonebookid = $t['phonebook'].'.id';

		if(($phonebookaddressjoin = $this->_dso->join($t['phonebookaddress'],
							      'LEFT',
							      $t['phonebookaddress'].'.phonebookid = '.$phonebookid)) === false
		|| ($phonebooknumberjoin = $this->_dso->join($t['phonebooknumber'],
							      'LEFT',
							      $t['phonebooknumber'].'.phonebookid = '.$phonebookid)) === false)
			return(false);

		$from = $t['phonebook'].
			' '.$phonebookaddressjoin.
			' '.$phonebooknumberjoin;

		$this->_dso->new_group('id',$t['phonebook']);
		$this->_dso->new_order($order,null,$t['phonebook']);
		$this->_dso->new_limit($limit);
	
		$r = $this->_dso->select_all($from,$t['phonebook'].'.*',$where);

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
