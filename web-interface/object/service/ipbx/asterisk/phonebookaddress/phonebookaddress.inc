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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_phonebookaddress extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'phonebookaddress';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_phonebookaddress(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get_all_type_by_phonebook($phonebookid,$prepare=true)
	{
		if(($phonebookid = dwho_ulongint($phonebookid)) === 0)
			return(false);

		$arr = array('phonebookid' => $phonebookid);

		if(($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		if((bool) $prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				if(isset($r[$list[$i]['type']]) === true)
					continue;

				$r[$list[$i]['type']] = $this->_prepare($list[$i]);
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
			{
				if(isset($r[$list[$i]['type']]) === true)
					continue;

				$r[$list[$i]['type']] = $list[$i];
			}
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
