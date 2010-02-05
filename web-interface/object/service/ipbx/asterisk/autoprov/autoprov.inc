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

class xivo_service_asterisk_autoprov extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'autoprov';
	var $_filter	= false;
	var $_autoprov	= null;
	var $_sockprov	= null;
	var $_list	= null;

	function xivo_service_asterisk_autoprov(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
		$this->_chk_autoprov();
	}

	function _chk_autoprov()
	{
		if(dwho_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['host'],$this->_sre->_ini[$this->_name]['port']) === false)
			$this->_autoprov = false;
		else
		{
			$this->_autoprov = $this->_conf['autoprov'];
			$this->_autoprov['prov']['address'] = $this->_sre->_ini[$this->_name]['host'];
			$this->_autoprov['prov']['port'] = $this->_sre->_ini[$this->_name]['port'];
			$this->_autoprov['list']['address'] = $this->_autoprov['prov']['address'];
			$this->_autoprov['list']['port'] = $this->_autoprov['prov']['port'];
		}
	}

	function _load_sockprov($type)
	{
		if(($type !== 'list' && $type !== 'prov') === true
		|| dwho_ak($type,$this->_autoprov) === false)
			return(false);
		else if(dwho_ak($type,$this->_sockprov) === true)
			return($this->_sockprov[$type]->open());

		dwho::load_class('dwho_socket');
		$this->_sockprov[$type] = new dwho_socket($this->_autoprov[$type]);

		if(dwho_issa('sndtimeo',$this->_autoprov[$type]) === true)
		{
			$this->_sockprov[$type]->set_option(null,
							    'sndtimeo',
							    $this->_autoprov[$type]['sndtimeo']);
		}

		if(dwho_issa('rcvtimeo',$this->_autoprov[$type]) === true)
		{
			$this->_sockprov[$type]->set_option(null,
							    'rcvtimeo',
							    $this->_autoprov[$type]['rcvtimeo']);
		}

		return(true);
	}

	function get_autoprov_list()
	{
		if($this->_list !== null)
			return($this->_list);
		else if($this->_load_sockprov('list') === false
		|| $this->_sockprov['list']->is_open() === false)
			return(($this->_list = false));
		else if(($write = $this->_mk_sockprov_http('list')) === false
		|| $this->_sockprov['list']->write($write) === false
		|| ($read = $this->_sockprov['list']->read_binary()) === false
		|| ($recv = $this->_parse_sockprov_recv($read)) === false
		|| isset($recv['status']) === false
		|| ($nb = count($recv['data'])) === 0
		|| preg_match('/^HTTP\/1\.[0-1] 200 /i',$recv['status']) !== 1)
		{
			$this->_sockprov['list']->shutdown();
			return(($this->_list = false));
		}

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$recv['data'][$i];

			if(preg_match('/^([0-9a-z\-_]+)="([0-9a-zA-Z\-_\. ]+)"$/',$ref,$match) === 1
			&& isset($r[$match[1]]) === false)
			{
				$r[$match[1]] = array();
				$r[$match[1]]['name'] = $match[2];
				$r[$match[1]]['model'] = array();
			}
			else if(preg_match('/^([0-9a-z\-_]+)\.([0-9a-z\-_]+)="([0-9a-zA-Z\-_\. ]+)"$/',$ref,$match) === 1
			&& isset($r[$match[1]]) === true)
			{
				$r[$match[1]]['model'][$match[2]] = array();
				$r[$match[1]]['model'][$match[2]]['path'] = $match[1].'.'.$match[2];
				$r[$match[1]]['model'][$match[2]]['meta'] = $match[2];
				$r[$match[1]]['model'][$match[2]]['label'] = $match[3];
			}
		}

		if(empty($r) === true)
			$r = false;
		else
			ksort($r);

		$this->_list = $r;

		return($r);
	}

	function _mk_sockprov_http($type,$data='')
	{
		$data = (string) $data;

		if(dwho_ak($type,$this->_autoprov) === false)
			return(false);

		$r  = strtoupper($this->_autoprov[$type]['method']).' ';
		$r .= $this->_autoprov[$type]['url'].' HTTP/1.1'."\r\n";
		$r .= 'Host: '.$this->_autoprov[$type]['address']."\r\n";

		if(($len = strlen($data)) !== 0)
		{
			$r .= 'Content-Length: '.$len."\r\n\r\n";
			$r .= $data;
		}
		else $r .= "\r\n";

		return($r);
	}

	function _parse_sockprov_recv($recv)
	{
		$recv = (string) $recv;

		$recv = str_replace("\r",'',$recv);
		$recv = explode("\n",$recv);

		if(($nb = count($recv)) === 0)
			return(false);

		$r = array();
		$r['data'] = array();
		$data = false;

		for($i = 0;$i < $nb;$i++)
		{
			$header = explode(': ',$recv[$i],2);

			if($i === 0 && $header[0] !== '' && isset($header[1]) === false)
				$r['status'] = $header[0];
			else if($data === false && $recv[$i] === '')
				$data = true;
			else if($data === false && isset($header[0],$header[1]) === true)
				$r[$header[0]] = $header[1];
			else if($i === ($nb - 1) && $recv[$i] === '')
				continue;
			else
				$r['data'][] = $recv[$i];
		}

		return($r);
	}

	function _send_sockprov_mode($mode,$arr,$modact='')
	{
		$mode = (string) $mode;
		$modact = (string) $modact;

		if(is_array($arr) === false)
			return(false);

		unset($arr['modact']);

		if($modact === 'guest')
			$arr['iduserfeatures'] = 0;

		$arr['mode'] = $mode;

		$data = '';

		foreach($arr as $key => $val)
		{
			if(dwho_has_len($val) === true)
				$data .= $key.'='.$val."\r\n";
		}

		if($this->_load_sockprov('prov') === false
		|| ($write = $this->_mk_sockprov_http('prov',$data)) === false
		|| $this->_sockprov['prov']->is_open() === false)
			return(false);
		else
			$r = $this->_sockprov['prov']->write($write) !== false;

		$this->_sockprov['prov']->shutdown();

		return($r);
	}

	function notification($arr,$mod)
	{
		if(is_array($arr) === false)
			return(false);

		unset($arr['vendor'],$arr['model']);

		$arr['actions'] = 'yes';

		if(($mod !== 'guest' && $mod !== 'prov') === true
		|| $this->_send_sockprov_mode('notification',$arr,$mod) === false)
			return(false);

		return(true);
	}

	function authoritative($arr,$mod)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['actions'] = 'no';
		$arr['isinalan'] = 0;

		if(($mod !== 'guest' && $mod !== 'prov') === true
		|| $this->_send_sockprov_mode('authoritative',$arr,$mod) === false)
			return(false);

		return(true);
	}

	function userdeleted($iduserfeatures)
	{
		if(($id = dwho_ulongint($iduserfeatures)) === 0)
			return(false);

		$r = array();
		$r['actions'] = 'no';
		$r['iduserfeatures'] = $id;

		if($this->_send_sockprov_mode('userdeleted',$r) === false)
			return(false);

		return(true);
	}
}

?>
