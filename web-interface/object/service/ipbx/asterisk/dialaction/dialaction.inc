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

class xivo_service_asterisk_dialaction extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'dialaction';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_dialaction(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _prepare($data)
	{
		$arr = array();
		$arr['action'] = '';
		$arr['actionarg1'] = '';
		$arr['actionarg2'] = '';

		$data['none'] = $arr;
		$data['endcall'] = $arr;
		$data['user'] = $arr;
		$data['group'] = $arr;
		$data['queue'] = $arr;
		$data['meetme'] = $arr;
		$data['voicemail'] = $arr;
		$data['schedule'] = $arr;
		$data['voicemenu'] = $arr;
		$data['extension'] = $arr;
		$data['application'] = $arr;
		$data['sound'] = $arr;
		$data['custom'] = $arr;

		$exp = explode(':',$data['action'],2);

		if(isset($data[$exp[0]]) === true)
		{
			if(isset($exp[1]) === false)
				$data[$exp[0]]['action'] = $exp[0];
			else
			{
				$data[$exp[0]]['action'] = $exp[1];
				$data[$exp[1]]['action'] = $exp[1];
				$data[$exp[1]]['actionarg1'] = $data['actionarg1'];
				$data[$exp[1]]['actionarg2'] = $data['actionarg2'];
			}

			$data[$exp[0]]['actionarg1'] = $data['actionarg1'];
			$data[$exp[0]]['actionarg2'] = $data['actionarg2'];

			switch($exp[0])
			{
				case 'sound':
					$data['sound']['actionarg2'] = array();

					if(strpos($data['actionarg2'],'skip') !== false)
						$data['sound']['actionarg2']['skip'] = true;

					if(strpos($data['actionarg2'],'noanswer') !== false)
						$data['sound']['actionarg2']['noanswer'] = true;

					if(strpos($data['actionarg2'],'j') !== false)
						$data['sound']['actionarg2']['j'] = true;
					break;
				case 'voicemail':
					$data['voicemail']['actionarg2'] = array();

					if(strpos($data['actionarg2'],'b') !== false)
						$data['voicemail']['actionarg2']['b'] = true;

					if(strpos($data['actionarg2'],'s') !== false)
						$data['voicemail']['actionarg2']['s'] = true;

					if(strpos($data['actionarg2'],'u') !== false)
						$data['voicemail']['actionarg2']['u'] = true;

					if(strpos($data['actionarg2'],'j') !== false)
						$data['voicemail']['actionarg2']['j'] = true;
					break;
			}
		}

		$data['action'] = $exp[0];
		$data['linked'] = (bool) $data['linked'];

		return($data);
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['action']) === true
		&& $arr['action'] === 'none')
			$arr['actionarg1'] = $arr['actionarg2'] = '';

		if(isset($arr['event']) === true)
			$arr['event'] = strval($arr['event']);

		return(parent::add($arr));
	}

	function edit($id,$arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['action']) === true
		&& $arr['action'] === 'none')
			$arr['actionarg1'] = $arr['actionarg2'] = '';

		if(is_array($id) === true && isset($id['event']) === true)
			$id['event'] = strval($id['event']);

		if(isset($arr['event']) === true)
			$arr['event'] = strval($arr['event']);

		return(parent::edit($id,$arr));
	}

	function linked($id)
	{
		return($this->edit($id,array('linked' => true)));
	}

	function unlinked($id)
	{
		return($this->edit($id,array('linked' => false)));
	}

	function linked_where($id)
	{
		return($this->edit_where($id,array('linked' => true)));
	}

	function unlinked_where($id)
	{
		return($this->edit_where($id,array('linked' => false)));
	}

	function is_valid($event,$category,$categoryval,$action,$actionarg1='',$actionarg2=null,$disable=null)
	{
		$event = strval($event);

		if($this->chk_element_value('category',$category) === false
		|| $this->chk_element_value('action',$action) === false
		|| $this->chk_categoryevent_value($category,$event) === false)
			return(false);
		else if($category === $action)
			$id = $categoryval;
		else
			$id = null;

		switch($action)
		{
			case 'none':
				return(true);
			case 'endcall:busy':
				return($this->chk_dest_endcall('busy',$actionarg1));
			case 'endcall:congestion':
				return($this->chk_dest_endcall('congestion',$actionarg1));
			case 'endcall:hangup':
				return($this->chk_dest_endcall('hangup'));
			case 'user':
				if(xivo_has_len($actionarg2) === true
				&& xivo_is_unumber($actionarg2) === false)
					return(false);

				return($this->chk_dest_user($actionarg1,$actionarg2,$id,$disable));
			case 'group':
				if(xivo_has_len($actionarg2) === true
				&& xivo_is_unumber($actionarg2) === false)
					return(false);

				return($this->chk_dest_group($actionarg1,$actionarg2,$id,$disable));
			case 'queue':
				if(xivo_has_len($actionarg2) === true
				&& xivo_is_unumber($actionarg2) === false)
					return(false);

				return($this->chk_dest_queue($actionarg1,$actionarg2,$id,$disable));
			case 'meetme':
				return($this->chk_dest_meetme($actionarg1,null,$disable));
			case 'voicemail':
				return($this->chk_dest_voicemail($actionarg1,$actionarg2,null,$disable));
			case 'schedule':
				return($this->chk_dest_schedule($actionarg1,$id,$disable));
			case 'voicemenu':
				return($this->chk_dest_voicemenu($actionarg1,null,$disable));
			case 'extension':
				return($this->chk_dest_extension($actionarg1,$actionarg2));
			case 'application:callbackdisa':
				return($this->chk_dest_application('callbackdisa',$actionarg1,$actionarg2));
			case 'application:disa':
				return($this->chk_dest_application('disa',$actionarg1,$actionarg2));
			case 'application:directory':
				return($this->chk_dest_application('directory',$actionarg1,$actionarg2));
			case 'application:faxtomail':
				return($this->chk_dest_application('faxtomail',$actionarg1,$actionarg2));
			case 'application:voicemailmain':
				return($this->chk_dest_application('voicemailmain',$actionarg1,$actionarg2));
			case 'sound':
				return($this->chk_dest_sound('playback',$actionarg1,$actionarg2));
			case 'custom':
				return($this->chk_dest_custom($actionarg1));
		}

		return(false);
	}

	function get_categoryevent_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| xivo_issa('categoryevent',$this->_conf) === false
		|| xivo_issa($key,$this->_conf['categoryevent']) === false)
			return(false);

		return($this->_conf['categoryevent'][$key]);
	}

	function chk_categoryevent_value($key,$value)
	{
		if(($values = $this->get_categoryevent_value($key)) === false
		|| in_array($value,$values,true) === false)
			return(false);

		return(true);
	}
}

?>
