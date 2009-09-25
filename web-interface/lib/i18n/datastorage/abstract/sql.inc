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

class dwho_i18n_abstract_sql
{
	var $_param	= array('table'	=> 'i18ncache');
	var $_dso	= null;

	function _init(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function save_cache($arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(($arr['dcreate'] = dwho_uint($arr['dcreate'])) === 0)
			$arr['dcreate'] = time();

		if(($arr['dupdate'] = dwho_uint($arr['dupdate'])) === 0)
			$arr['dupdate'] = time();

		$arr['obj'] = serialize($arr['obj']);

		return($this->_dso->replace($this->_param['table'],$arr));
	}

	function get_cache($arr)
	{
		if(is_array($arr) === false)
			return(false);

		$this->_dso->new_select($this->_param['table'],array('dcreate','dupdate','obj'));
		$this->_dso->where(array('locale' => $arr['locale'],
					 'path'	=> $arr['path']));

		if(($r = $this->_dso->select_single()) !== false)
		{
			$r['filectime'] = $r['dcreate'];

			if((int) $r['dupdate'] === (int) $arr['filemtime'])
				$r['obj'] = dwho_unserialize($r['obj']);
		}

		return($r);
	}
}

?>
