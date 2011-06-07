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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_campaign_general extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'campaign_general';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_campaign_general(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id,$disable=null)
	{
		$r = parent::get($id,$disable);
		if($r !== false)
		{
			foreach(array('svichoices','svientries','svivariables') as $key)
			{
				$items = array();
				foreach(split(';', $r[$key]) as $item)
				{ 
					$item = split('=', $item);
					$items[$item[0]] = $item[1];
				}

				$r[$key] = $items;
			}
		}

		// convert seconds to days
		$r['purge_syst_tagged_delay'] /= 86400;
		$r['purge_syst_untagged_delay'] /= 86400;
		$r['purge_punct_delay'] /= 86400;

		$r['purge_syst_tagged_at']   = substr($r['purge_syst_tagged_at'], 0, 5);
		$r['purge_syst_untagged_at'] = substr($r['purge_syst_untagged_at'], 0, 5);
		$r['purge_punct_at']         = substr($r['purge_punct_at'], 0, 5);

		return $r;
	}

	function edit($id,$arr,$disable=null)
	{
		foreach(array('svichoices','svientries','svivariables') as $key)
		{
			$items = array();			
			foreach($arr[$key] as $k => $v)
				$items[] = "$k=$v";

			$arr[$key] = implode(';', $items);
		}

		// convert days to seconds
		$arr['purge_syst_tagged_delay']   *= 86400;
		$arr['purge_syst_untagged_delay'] *= 86400;
		$arr['purge_punct_delay']         *= 86400;

		return parent::edit($id,$arr,$disable);
	}
}

?>
