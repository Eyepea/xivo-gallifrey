<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_webservices_phonebook_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'phonebook'		=> '',
				'phonebooknumber'	=> '');

	function xivo_application_service_asterisk_webservices_phonebook_sql(&$dso,&$dsoconf)
	{
		$this->_init(&$dso,&$dsoconf);
	}

	function search($value,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		$this->_dso->new_select($this->_table['phonebook'],array('name' => 'displayname'));
		$this->_dso->innerjoin($this->_table['phonebooknumber'],
				       $this->_table['phonebook'].'.id = '.$this->_table['phonebooknumber'].'.phonebookid',
				       null,
				       array('phone' => 'number','type'));
		$this->_dso->wherecontain('number',$value,$this->_table['phonebooknumber']);
		$this->_dso->orwherecontain('firstname',$value,$this->_table['phonebook']);
		$this->_dso->orwherecontain('lastname',$value,$this->_table['phonebook']);
		$this->_dso->orwherecontain('displayname',$value,$this->_table['phonebook']);

		if($this->_dso->aggregate_where() === false)
			return(false);

		$this->_dso->andwherenot(array('type' => 'fax'),$this->_table['phonebooknumber']);

		$this->_dso->new_group('id',$this->_table['phonebooknumber']);
		$this->_dso->new_order($order,null,$this->_table['phonebook']);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_group();
		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count(null,'id',$this->_table['phonebook'])) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
