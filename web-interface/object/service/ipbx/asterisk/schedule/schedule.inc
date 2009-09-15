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

class xivo_service_asterisk_schedule extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'schedule';
	var $_filter	= false;
	var $_origin	= false;
	var $_identity	= false;

	function xivo_service_asterisk_schedule(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
		$this->_identity = &$this->_sre->get_identity($this->_name);
	}

	function _prepare($data)
	{
		$this->_identity->_prepare_identity($data);

		$data['timebeg'] = (string) $data['timebeg'];
		$data['timeend'] = (string) $data['timeend'];

		$data['publicholiday'] = (bool) $data['publicholiday'];
		$data['commented'] = (bool) $data['commented'];

		if($data['timebeg'] === '' || $data['timebeg'] === '*')
		{
			$data['timehourbeg'] = $data['timeminbeg'] = '';
			$data['timeend'] = $data['timehourend'] = $data['timeminend'] = '';

			return($data);
		}

		$hour = explode(':',$data['timebeg'],3);

		$data['timehourbeg'] = sprintf('%02u',$hour[0]);

		if(xivo_has_len($hour,1) === true)
			$data['timeminbeg'] = sprintf('%02u',$hour[1]);
		else
			$data['timeminbeg'] = '00';

		if($data['timeend'] === '' || $data['timeend'] === '*')
		{
			$data['timeend'] = $data['timehourend'] = $data['timeminend'] = '';
			return($data);
		}

		$hour = explode(':',$data['timeend'],3);

		$data['timehourend'] = sprintf('%02u',$hour[0]);

		if(xivo_has_len($hour,1) === true)
			$data['timeminend'] = sprintf('%02u',$hour[1]);
		else
			$data['timeminend'] = '00';

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['timebeg'],$arr['timeend']) === true
		&& $arr['timebeg'] === $arr['timeend'])
			$arr['timeend'] = null;

		if(isset($arr['daynamebeg'],$arr['daynameend']) === true
		&& $arr['daynamebeg'] === $arr['daynameend'])
			$arr['daynameend'] = null;

		if(isset($arr['daynumbeg'],$arr['daynumend']) === true
		&& $arr['daynumbeg'] === $arr['daynumend'])
			$arr['daynumend'] = null;

		if(isset($arr['monthbeg'],$arr['monthend']) === true
		&& $arr['monthbeg'] === $arr['monthend'])
			$arr['monthend'] = null;

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['timebeg'],$arr['timeend']) === true
		&& $arr['timebeg'] === $arr['timeend'])
			$arr['timeend'] = null;

		if(isset($arr['daynamebeg'],$arr['daynameend']) === true
		&& $arr['daynamebeg'] === $arr['daynameend'])
			$arr['daynameend'] = null;

		if(isset($arr['daynumbeg'],$arr['daynumend']) === true
		&& $arr['daynumbeg'] === $arr['daynumend'])
			$arr['daynumend'] = null;

		if(isset($arr['monthbeg'],$arr['monthend']) === true
		&& $arr['monthbeg'] === $arr['monthend'])
			$arr['monthend'] = null;

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr,$disable));
	}

	function mk_time($beg,$end)
	{
		$r = array('beg' => '*','end' => '');

		if(is_array($beg) === false || is_array($end) === false
		|| isset($beg['hour'],$beg['min'],$end['hour'],$end['min']) === false)
			return(false);

		$beg['hour'] = strval($beg['hour']);
		$beg['min'] = strval($beg['min']);

		$end['hour'] = strval($end['hour']);
		$end['min'] = strval($end['min']);

		$begint = $beg['hour'].$beg['min'];

		if($begint === '' || strpos($begint,'*') !== false)
			return($r);
		else if(ctype_digit($begint) === false)
			return(false);

		$r['beg'] = sprintf('%02u:%02u',$beg['hour'],$beg['min']);

		$endint = $end['hour'].$end['min'];

		if($endint === '')
			return($r);
		else if(ctype_digit($endint) === false || (int) $begint > (int) $endint)
			return(false);

		$r['end'] = sprintf('%02u:%02u',$end['hour'],$end['min']);

		return($r);
	}

	function chk_date($arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['daynamebeg']) === false
		|| $arr['daynamebeg'] === ''
		|| $arr['daynamebeg'] === '*')
		{
			$arr['daynamebeg'] = '*';
			$arr['daynameend'] = null;
		}

		if(isset($arr['daynumbeg']) === false
		|| $arr['daynumbeg'] === ''
		|| $arr['daynumbeg'] === '*')
		{
			$arr['daynumbeg'] = '*';
			$arr['daynumend'] = null;
		}

		if(isset($arr['monthbeg']) === false
		|| $arr['monthbeg'] === ''
		|| $arr['monthbeg'] === '*')
		{
			$arr['monthbeg'] = '*';
			$arr['monthend'] = null;
		}

		if($arr['daynumbeg'] === '*' || $arr['monthbeg'] === '*')
			return($arr);

		$leapdbeg = strtolower($arr['daynumbeg'].'-'.$arr['monthbeg'].'-2000');
		$normdbeg = strtolower($arr['daynumbeg'].'-'.$arr['monthbeg'].'-2001');

		$leapdate = strtolower(date('j-M-Y',strtotime($arr['daynumbeg'].' '.$arr['monthbeg'].' 2000')));
		$normdate = strtolower(date('j-M-Y',strtotime($arr['daynumbeg'].' '.$arr['monthbeg'].' 2001')));

		if($leapdbeg !== $leapdate && $normdbeg !== $normdate)
			return(false);
		else if($arr['daynumend'] === null || $arr['monthend'] === null)
			return($arr);

		$leapdend = strtolower($arr['daynumend'].'-'.$arr['monthend'].'-2000');
		$normdend = strtolower($arr['daynumend'].'-'.$arr['monthend'].'-2001');

		$leapdate = strtolower(date('j-M-Y',strtotime($arr['daynumend'].' '.$arr['monthend'].' 2000')));
		$normdate = strtolower(date('j-M-Y',strtotime($arr['daynumend'].' '.$arr['monthend'].' 2001')));

		if($leapdend !== $leapdate && $normdend !== $normdate)
			return(false);

		return($arr);
	}

	function get_all_except($id,$order=null,$limit=null)
	{
		if(($id = xivo_ulongint($id)) === 0)
			return($this->get_all(null,true,$order,$limit));

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(($list = $this->_dso->get_all_except($id,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);
		else if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		return($this->_mk_list_assoc_prepare($list,false,true));
	}
}

?>
