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

class xivo_service_asterisk_cdr extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'cdr';
	var $_filter	= false;

	function xivo_service_asterisk_cdr(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function search($arr,$order=null,$limit=null)
	{
		if(is_array($arr) === false)
			return(false);

		$search = array();

		if(xivo_has_len($arr,'dbeg') === true)
		{
			$date = array('dbeg' => null,'dend' => null);

			if(xivo_has_len($arr,'dend') === true
			&& ($date = xivo_filter::chk_between_date($arr['dbeg'],$arr['dend'])) === false)
				return(false);
			else if(($date['dbeg'] = xivo_filter::chk_date($arr['dbeg'])) === false)
				return(false);

			if($date['dbeg']['dateint'] > strftime($date['dbeg']['format']))
				return(false);
			else
				$search['dbeg'] = $date['dbeg']['date'];

			if($date['dend'] !== null && $date['dend']['dateint'] > strftime($date['dend']['format']))
				return(false);
			else
				$search['dend'] = $date['dend']['date'];
		}

		if(xivo_has_len($arr,'channel') === true)
			$search['channel'] = $this->_search_channel($arr['channel']);

		if(xivo_has_len($arr,'disposition') === true)
			$search['disposition'] = $arr['disposition'];

		if(xivo_has_len($arr,'amaflags') === true)
			$search['amaflags'] = $arr['amaflags'];

		if(xivo_has_len($arr,'dcontext') === true)
			$search['dcontext'] = $arr['dcontext'];

		if(xivo_has_len($arr,'src') === true && isset($arr['srcformat']) === true)
			$search['src'] = array('pattern' => $arr['src'],'type' => $arr['srcformat']);

		if(xivo_has_len($arr,'dst') === true && isset($arr['dstformat']) === true)
			$search['dst'] = array('pattern' => $arr['dst'],'type' => $arr['dstformat']);

		if(xivo_has_len($arr,'clid') === true && isset($arr['clidformat']) === true)
			$search['clid'] = array('pattern' => $arr['clid'],'type' => $arr['clidformat']);

		if(xivo_has_len($arr,'accountcode') === true && isset($arr['accountcodeformat']) === true)
			$search['accountcode'] = array('pattern' => $arr['accountcode'],'type' => $arr['accountcodeformat']);

		if(xivo_has_len($arr,'userfield') === true && isset($arr['userfieldformat']) === true)
			$search['userfield'] = array('pattern' => $arr['userfield'],'type' => $arr['userfieldformat']);

		if(xivo_has_len($arr,'dubeg') === true && isset($arr['dubegunit']) === true)
		{
			if($arr['dubegunit'] === 'minutes')
				$dubeg = $arr['dubeg'] * 60;
			else
				$dubeg = $arr['dubeg'];

			if(xivo_has_len($arr,'duend') === true && isset($arr['duendunit']) === true)
			{
				if($arr['duendunit'] === 'minutes')
					$duend = $arr['duend'] * 60;
				else
					$duend = $arr['duend'];

				if($dubeg <= $duend)
					$search['duend'] = $duend;
			}
			$search['dubeg'] = $dubeg;
		}

		if(($result = $this->_dso->search($search,$order,$limit)) === false)
			return(null);

		$nb = count($result);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$result[$i];

			if(isset($ref['amaflags']) === false)
				$ref['amaflags'] = 0;

			$ref['amaflagsmeta'] = $this->amaflags_meta($ref['amaflags']);
		}

		return($result);
	}

	function _search_channel($channel)
	{
		if($channel !== 'others')
			return($channel);

		$channel = $this->get_element_value('channel');
		unset($channel['others']);
		$channel = array_keys($channel);

		return($channel);
	}

	function amaflags_meta($int)
	{
		$int = xivo_uint($int);

		$amaflags = $this->get_element_value('amaflags');

		if(isset($amaflags[$int]) === true)
			return($amaflags[$int]);

		return(XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT);
	}

	function channel_meta($str)
	{
		$str = (string) $str;

		$channels = $this->get_element_value('channel');

		if(isset($channels[$str]) === true)
			return($channels[$str]);

		return(false);
	}
}

?>
