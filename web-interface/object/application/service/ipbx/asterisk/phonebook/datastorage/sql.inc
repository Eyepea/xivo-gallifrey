<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'application','abstract','datastorage','sql.inc'));

class xivo_application_service_asterisk_phonebook_sql extends xivo_application_abstract_datastorage_sql
{
	var $_table	= array(
				'phonebook'		=> '',
				'phonebookaddress'	=> '',
				'phonebooknumber'	=> '');

	function search($value,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		if(dwho_has_len($value) === false)
			return(false);

		$this->_dso->new_select($this->_table['phonebook']);

		$this->_dso->leftjoin($this->_table['phonebookaddress'],
				      $this->_table['phonebook'].'.id = '.$this->_table['phonebookaddress'].'.phonebookid');

		$this->_dso->leftjoin($this->_table['phonebooknumber'],
				      $this->_table['phonebook'].'.id = '.$this->_table['phonebooknumber'].'.phonebookid');

		$this->_dso->wherecontain('address1',$value,true,$this->_table['phonebookaddress']);
		$this->_dso->orwherecontain('address2',$value,true,$this->_table['phonebookaddress']);
		$this->_dso->orwherecontain('city',$value,true,$this->_table['phonebookaddress']);
		$this->_dso->orwherecontain('state',$value,true,$this->_table['phonebookaddress']);
		$this->_dso->orwherecontain('zipcode',$value,true,$this->_table['phonebookaddress']);
		$this->_dso->orwherecontain('country',$value,true,$this->_table['phonebookaddress']);
		$this->_dso->orwherecontain('number',$value,true,$this->_table['phonebooknumber']);
		$this->_dso->orwherecontain('firstname',$value,true,$this->_table['phonebook']);
		$this->_dso->orwherecontain('lastname',$value,true,$this->_table['phonebook']);
		$this->_dso->orwherecontain('displayname',$value,true,$this->_table['phonebook']);
		$this->_dso->orwherecontain('society',$value,true,$this->_table['phonebook']);
		$this->_dso->orwherecontain('email',$value,true,$this->_table['phonebook']);
		$this->_dso->orwherecontain('url',$value,true,$this->_table['phonebook']);
		$this->_dso->orwherecontain('description',$value,true,$this->_table['phonebook']);

		if($this->_dso->aggregate_where() === false)
			return(false);

		$this->_dso->new_group('id',$this->_table['phonebook']);
		$this->_dso->new_order($order,null,$this->_table['phonebook']);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_group();
		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_distinct_count_singlecolumn(
						null,
						'id',
						$this->_table['phonebook'])) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
