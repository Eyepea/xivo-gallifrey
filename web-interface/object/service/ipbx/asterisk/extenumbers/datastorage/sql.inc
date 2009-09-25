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

		if(dwho_has_len($context) === true)
			$where['context'] = $context;

		if(dwho_has_len($type) === true)
			$where['type'] = $type;

		if(dwho_has_len($typeval) === true)
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
