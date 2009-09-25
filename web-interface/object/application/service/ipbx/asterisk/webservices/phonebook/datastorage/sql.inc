<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_webservices_phonebook_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'phonebook'		=> '',
				'phonebooknumber'	=> '');

	function search($value,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		$this->_dso->new_select($this->_table['phonebook'],array('name' => 'displayname'));
		$this->_dso->innerjoin($this->_table['phonebooknumber'],
				       $this->_table['phonebook'].'.id = '.$this->_table['phonebooknumber'].'.phonebookid',
				       null,
				       array('phone' => 'number','type'));
		$this->_dso->wherecontain('number',$value,true,$this->_table['phonebooknumber']);
		$this->_dso->orwherecontain('firstname',$value,true,$this->_table['phonebook']);
		$this->_dso->orwherecontain('lastname',$value,true,$this->_table['phonebook']);
		$this->_dso->orwherecontain('displayname',$value,true,$this->_table['phonebook']);

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
