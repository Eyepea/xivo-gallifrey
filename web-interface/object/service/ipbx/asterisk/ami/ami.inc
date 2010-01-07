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

class xivo_service_asterisk_ami extends xivo_service_asterisk_abstract
{
	var $_name	= 'ami';
	var $_socket	= null;

	function xivo_service_asterisk_ami(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;

		$this->_load_config();

		if($this->_chk_socket() === false)
			trigger_error('Invalid or missing configuration in '.$this->_sre->get_type().' init file',E_USER_ERROR);
	}

	function _chk_socket()
	{
		if(dwho_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['bindaddr'],$this->_sre->_ini[$this->_name]['port']) === false)
			return(false);

		$this->_conf['socket']['address'] = $this->_sre->_ini[$this->_name]['bindaddr'];
		$this->_conf['socket']['port'] = $this->_sre->_ini[$this->_name]['port'];
	}

	function _load_socket()
	{
		if($this->_socket !== null)
			return($this->_socket->open());

		dwho::load_class('dwho_socket');
		$this->_socket = new dwho_socket($this->_conf['socket']);

		return(true);
	}

	function _set_options()
	{
		if($this->_socket->is_open() === false)
			return(false);

		if(dwho_issa('sndtimeo',$this->_conf['socket']) === true)
		{
			$this->_socket->set_option(null,
						   'sndtimeo',
						   $this->_conf['socket']['sndtimeo']);
		}

		if(dwho_issa('rcvtimeo',$this->_conf['socket']) === true)
		{
			$this->_socket->set_option(null,
						   'rcvtimeo',
						   $this->_conf['socket']['rcvtimeo']);
		}

		return(true);
	}

	function cmd($cmd,$read=false)
	{
		$cmd = trim($cmd);

		if(dwho_is_sgraph($cmd) === false
		|| $this->_load_socket() === false
		|| $this->_set_options() === false)
			return(false);
		else if(($r = $this->_socket->write($cmd)) !== false && (bool) $read === true)
			$r = $this->_parse_cti_recv($this->_socket->read_binary());

		$this->_socket->shutdown();

		return($r);
	}

	function _parse_cti_recv($recv)
	{
		$recv = (string) $recv;
		$recv = str_replace("\r",'',$recv);
		$recv = explode("\n",$recv);

		if(($nb = count($recv)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$msg = explode(':',$recv[$i],2);

			if(isset($msg[1]) === false)
				$msg[1] = '';

			if($msg[0] === 'XIVO-CLI-WEBI')
			{
				if(substr($msg[1],0,2) === 'KO')
					return(false);
				continue;
			}
			else if($msg[0] === 'Privilege')
				continue;

			$r[] = $recv[$i];
		}

		if(end($r) === '')
			array_pop($r);

		return($r);
	}
}

?>
