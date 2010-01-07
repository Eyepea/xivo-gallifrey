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

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_cdr_sql extends xivo_service_asterisk_abstract_sql
{
	var $_disable	= null;

	function search($arr,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		$this->_dso->new_select($this->_table);

		if($this->_search_where($arr) === false)
			return(false);

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count()) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function _search_where($arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['dbeg']) === true)
		{
			$dbeg = $arr['dbeg'];
			$len = strlen($dbeg);

			if($len === 4)
				$format = '%Y';
			else if($len === 7)
				$format = '%Y-%m';
			else
				$format = '%Y-%m-%d';

			if(isset($arr['dend']) === true)
				$dend = $arr['dend'];
			else
				$dend = '';

			$this->_dso->wheredate('calldate',$dbeg,$dend,null,$format);
		}

		if(isset($arr['channel']) === true)
		{
			if(is_array($arr['channel']) === true)
				$this->_dso->andwherenotbegin('channel',$arr['channel']);
			else
				$this->_dso->andwherebegin('channel',$arr['channel']);
		}

		if(isset($arr['disposition']) === true)
			$this->_dso->andwhereexact('disposition',$arr['disposition']);

		if(isset($arr['amaflags']) === true)
			$this->_dso->andwhereexact('amaflags',$arr['amaflags']);

		if(isset($arr['dcontext']) === true)
			$this->_dso->andwhereexact('dcontext',$arr['dcontext']);

		if(dwho_issa('src',$arr) === true)
			$this->_dso->andwherelike('src',$arr['src']['pattern'],$arr['src']['type']);

		if(dwho_issa('dst',$arr) === true)
			$this->_dso->andwherelike('dst',$arr['dst']['pattern'],$arr['dst']['type']);

		if(dwho_issa('clid',$arr) === true)
			$this->_dso->andwherelike('clid',$arr['clid']['pattern'],$arr['clid']['type']);

		if(dwho_issa('accountcode',$arr) === true)
			$this->_dso->andwherelike('accountcode',$arr['accountcode']['pattern'],$arr['accountcode']['type']);

		if(dwho_issa('userfield',$arr) === true)
			$this->_dso->andwherelike('userfield',$arr['userfield']['pattern'],$arr['userfield']['type']);

		if(isset($arr['dubeg']) === true)
		{
			if(isset($arr['duend']) === true)
				$this->_dso->andwherebetween('duration',$arr['dubeg'],$arr['duend']);
			else
				$this->_dso->andwhereexact('duration',$arr['dubeg']);
		}

		return(true);
	}
}

?>
