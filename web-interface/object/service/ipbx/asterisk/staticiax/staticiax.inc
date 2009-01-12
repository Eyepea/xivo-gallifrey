<?php

#
# XiVO Web-Interface
# Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_staticiax extends xivo_service_asterisk_realstatic
{
	var $_filename	= 'iax.conf';

	function _prepare($data)
	{
		if($data['category'] === 'general'
		&& $data['var_name'] === 'register'
		&& is_scalar($data['var_val']) === true
		&& ($register = $this->parse_register($data['var_val'])) !== false)
			$data = array_merge($register,$data);

		return(parent::_prepare($data));
	}

	function parse_register($val)
	{
		if(preg_match('#^((?:[a-z0-9_\.-]+)(?:@[a-z0-9\.-]+)?)'.
			      '(:[a-z0-9_\.-]+)?'.
			      '@([a-z0-9\.-]+)(:[0-9]{1,5})?$#i',$val,$match) !== 1)
			return(false);

		$r = array();

		$r['username'] = $match[1];
		$r['host'] = $match[3];

		if(isset($match[2]) === true && $match[2] !== '')
			$r['password'] = substr($match[2],1);
		else
			$r['password'] = '';

		if(isset($match[4]) === true && $match[4] !== '')
			$r['port'] = substr($match[4],1);
		else
			$r['port'] = '';

		return($r);
	}
}

?>
