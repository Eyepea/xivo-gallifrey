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

class xivo_service_asterisk_phonefunckey extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'phonefunckey';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;
	var $_type_list		= false;

	function xivo_service_asterisk_phonefunckey(&$sre,&$dso)
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
		$data['type'] = '';
		$data['typeval'] = '';
		$data['user'] = '';
		$data['group'] = '';
		$data['queue'] = '';
		$data['meetme'] = '';
		$data['custom'] = '';
		$data['extension'] = false;
		$data['supervision'] = (bool) $data['supervision'];
		$data['progfunckey'] = (bool) $data['progfunckey'];

		switch($data['typeextenumbers'])
		{
			case 'extenfeatures':
				switch($data['typevalextenumbers'])
				{
					case 'bsfilter':
						$data['type'] = $data['typeextenumbers'].'-'.$data['typevalextenumbers'];
						$data['typeval'] = $data['typevalextenumbersright'];
						return($data);
					case 'extenfeatures-agentdynamiclogin':
					case 'extenfeatures-agentstaticlogin':
					case 'extenfeatures-agentstaticlogoff':
					case 'extenfeatures-agentstaticlogtoggle':
					case 'groupaddmember':
					case 'groupremovemember':
					case 'grouptogglemember':
					case 'queueaddmember':
					case 'queueremovemember':
					case 'queuetogglemember':
						$data['type'] = $data['typeextenumbers'].'-'.$data['typevalextenumbers'];
						$data['typeval'] = $data['typevalextenumbersright'];
						return($data);
				}
			case 'featuremap':
			case 'generalfeatures':
				if($data['type'] === '')
					$data['type'] = $data['typeextenumbers'].'-'.$data['typevalextenumbers'];

				if(dwho_issa($data['type'],$this->_conf['type']) === true)
				{
					$data['extension'] = (bool) dwho_ak('extension',$this->_conf['type'][$data['type']],true);

					if((bool) dwho_ak('destination',$this->_conf['type'][$data['type']],true) === true)
						$data['typeval'] = $data['exten'];
				}
				return($data);
		}

		switch($data['typeextenumbersright'])
		{
			case 'user':
			case 'group':
			case 'queue':
			case 'meetme':
				$data['type'] = $data['typeextenumbersright'];
				$data[$data['typeextenumbersright']] = $data['typeval'] = $data['typevalextenumbersright'];
				return($data);
		}

		if(dwho_has_len($data['exten']) === true || $data['type'] === '')
		{
			$data['type'] = 'custom';
			$data['custom'] = $data['typeval'] = $data['exten'];
		}

		return($data);
	}

	function get_type_list()
	{
		return($this->_conf['type']);
	}

	function mk_values_array($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['fknum'],$arr['type'],$arr['typeval'],$arr['iduserfeatures']) === false
		|| ($type_info = $this->_chk_exists_cfg('type',$arr['type'],true)) === false)
			return(false);

		$r = array();
		$r['iduserfeatures'] = $arr['iduserfeatures'];
		$r['fknum'] = $arr['fknum'];
		$r['exten'] = '';
		$r['typeextenumbers'] = '';
		$r['typevalextenumbers'] = '';
		$r['typeextenumbersright'] = '';
		$r['typevalextenumbersright'] = '';

		if(isset($arr['label']) === true)
			$r['label'] = $arr['label'];
		else
			$r['label'] = '';

		if(isset($type_info['supervisable']) === true
		&& (bool) $type_info['supervisable'] === true
		&& isset($arr['supervision']) === true)
			$r['supervision'] = intval((bool) $arr['supervision']);
		else
			$r['supervision'] = 0;

		if(isset($type_info['progfunckey']) === true)
			$r['progfunckey'] = intval((bool) $type_info['progfunckey']);
		else
			$r['progfunckey'] = 0;

		$type = explode('-',$arr['type'],2);

		switch($arr['type'])
		{
			case 'extenfeatures-agentdynamiclogin':
			case 'extenfeatures-agentstaticlogin':
			case 'extenfeatures-agentstaticlogoff':
			case 'extenfeatures-agentstaticlogtoggle':
			case 'extenfeatures-groupaddmember':
			case 'extenfeatures-groupremovemember':
			case 'extenfeatures-grouptogglemember':
			case 'extenfeatures-queueaddmember':
			case 'extenfeatures-queueremovemember':
			case 'extenfeatures-queuetogglemember':
				if(dwho_has_len($arr['typeval']) === false)
					return(false);

				$r['typeextenumbers'] = 'extenfeatures';
				$r['typevalextenumbers'] = $type[1];
				$r['typeextenumbersright'] = substr($type[1],0,5);
				$r['typevalextenumbersright'] = $arr['typeval'];
				return($r);
			case 'extenfeatures-bsfilter':
				if(dwho_has_len($arr['typeval']) === false)
					return(false);

				$r['typeextenumbers'] = 'extenfeatures';
				$r['typevalextenumbers'] = $type[1];
				$r['typeextenumbersright'] = 'user';
				$r['typevalextenumbersright'] = $arr['typeval'];
				return($r);
		}

		switch($type[0])
		{
			case 'extenfeatures':
			case 'generalfeatures':
			case 'featuremap':
				if(isset($type_info['destination']) === true
				&& (bool) $type_info['destination'] === true)
					$r['exten'] = $arr['typeval'];

				$r['typeextenumbers'] = $type[0];
				$r['typevalextenumbers'] = $type[1];
				break;
			case 'custom':
				if(dwho_has_len($arr['typeval']) === false)
					return(false);

				$r['exten'] = $arr['typeval'];
				break;
			default:
				if(dwho_has_len($arr['typeval']) === false)
					return(false);

				$r['typeextenumbersright'] = $arr['type'];
				$r['typevalextenumbersright'] = $arr['typeval'];
		}

		return($r);
	}

	function is_valid($type='',$typeval='',$typeright='',$typevalright='',$id=null,$disable=null,$context=null)
	{
		switch($type)
		{
			case 'extenfeatures':
				switch($typeval)
				{
					case 'bsfilter':
						return(true);
					case 'agentdynamiclogin':
					case 'agentstaticlogin':
					case 'agentstaticlogoff':
					case 'agentstaticlogtoggle':
					case 'groupaddmember':
					case 'groupremovemember':
					case 'grouptogglemember':
					case 'queueaddmember':
					case 'queueremovemember':
					case 'queuetogglemember':
						if($typeright !== substr($typeval,0,5))
							return(false);
						break;
				}
			case 'featuremap':
			case 'generalfeatures':
				if($this->_chk_exists_cfg('type',$type.'-'.$typeval) === false)
					return(false);
				else if(dwho_has_len($typeright) === false)
					return(true);
				break;
			default:
				if(dwho_has_len($type) === true)
					return(false);
		}

		switch($typeright)
		{
			case 'agent':
				if($type !== 'extenfeatures'
				|| ($typeval !== 'agentdynamiclogin'
				   && $typeval !== 'agentstaticlogin'
				   && $typeval !== 'agentstaticlogoff'
				   && $typeval !== 'agentstaticlogtoggle') === true)
					return(false);

				return($this->chk_dest_agent($typevalright,null,null,$disable,false,true,$context));
			case 'user':
				return($this->chk_dest_user($typevalright,null,$id,$disable,false,true,$context));
			case 'group':
				return($this->chk_dest_group($typevalright,null,null,$disable,false,true,$context));
			case 'queue':
				return($this->chk_dest_queue($typevalright,null,null,$disable,false,true,$context));
			case 'meetme':
				return($this->chk_dest_meetme($typevalright,null,null,$disable,false,true,$context));
		}

		return(false);
	}
}

?>
