<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

class xivo_monit
{
	var $_url		= 'https://localhost:443/monit/';
	var $_statusurl		= 'https://localhost:443/monit/_status?format=json';
	var $_option		= array('ssl_verifypeer' => false);
	var $_curl		= null;
	var $_uri		= null;
	var $_conf		= false;
	var $_monit		= false;
	var $_service		= array();
	var $_group		= array();
	var $_device		= array();
	var $_directory		= array();
	var $_file		= array();
	var $_process		= array();
	var $_remotehost	= array();
	var $_fifo		= array();
	var $_status		= array('accessible',
					'accessible',
					'accessible',
					'running',
					'online',
					'running',
					'accessible');
	var $_types		= array('Device',
					'Directory',
					'File',
					'Process',
					'Remote Host',
					'System',
					'Fifo');
	var $_monitor		= array('notmonitored',
					'monitored',
					'initializing');
	var $_events		= array(0x00		=> 'noevent',
					0x01		=> 'checksum',
					0x02		=> 'resource',
					0x04		=> 'timeout',
					0x08		=> 'timestamp',
					0x10		=> 'size',
					0x20		=> 'connection',
					0x40		=> 'permission',
					0x80		=> 'uid',
					0x100		=> 'gid',
					0x200		=> 'noexist',
					0x400		=> 'invalid',
					0x800		=> 'data',
					0x1000		=> 'exec',
					0x2000		=> 'changed',
					0x4000		=> 'icmp',
					0x8000		=> 'match',
					0x10000		=> 'instance',
					0xFFFFFFFF	=> 'all');

	function xivo_monit($conf=null)
	{
		if(xivo::load_class('xivo_curl') === false
		|| xivo::load_class('xivo_json') === false)
			return(false);

		$this->_curl = new xivo_curl();

		if(is_array($conf) === false)
			return(($this->_conf = null));

		$this->_conf = $conf;

		if(xivo_issa('general',$this->_conf) === false)
			return(null);

		if(xivo_haslen($this->_conf['general'],'url') === true)
			$this->_url = rtrim($this->_conf['general']['url'],'/').'/';

		if(xivo_haslen($this->_conf['general'],'statusurl') === true)
			$this->_statusurl = $this->_conf['general']['statusurl'];
	}

	function get_monit()
	{
		return($this->_monit);
	}

	function get_service($name=null)
	{
		if(xivo_haslen($name) === false)
			return($this->_service);
		else if(isset($this->_service[$name]) === true)
			return($this->_service[$name]);

		return(false);
	}

	function get_device()
	{
		if(empty($this->_device) === false)
			return($this->_device);

		return(false);
	}

	function get_directory()
	{
		if(empty($this->_directory) === false)
			return($this->_directory);

		return(false);
	}

	function get_file()
	{
		if(empty($this->_file) === false)
			return($this->_file);

		return(false);
	}

	function get_process()
	{
		if(empty($this->_process) === false)
			return($this->_process);

		return(false);
	}

	function get_remotehost()
	{
		if(empty($this->_remotehost) === false)
			return($this->_remotehost);

		return(false);
	}

	function get_fifo()
	{
		if(empty($this->_fifo) === false)
			return($this->_fifo);

		return(false);
	}

	function get_system()
	{
		if(empty($this->_system) === false)
			return($this->_system);

		return(false);
	}

	function get_group($name=null)
	{
		if(xivo_haslen($name) === false)
			return($this->_group);
		else if(isset($this->_group[$name]) === true)
			return($this->_group[$name]);

		return(false);
	}

	function get_group_undefined()
	{
		return($this->get_group('__undefined__'));
	}

	function _manage_service($name,$action)
	{
		if(xivo_issa('service',$this->_conf) === true
		&& isset($this->_conf['service'][$name]) === true
		&& (bool) $this->_conf['service'][$name] === false)
			return(false);

		$r = $this->_curl->load($this->_url.$name.'?action='.$action,$this->_option);

		$this->_curl->close();

		return($r);
	}

	function is_monitorable($name)
	{
		if(isset($this->_service[$name]) === false
		|| $this->_service[$name]['monitor'] !== 0)
			return(false);
		else if(xivo_issa('monitorable',$this->_conf) === true
		&& isset($this->_conf['monitorable'][$name]) === true
		&& (bool) $this->_conf['monitorable'][$name] === false)
			return(false);

		return(true);
	}

	function is_unmonitorable($name)
	{
		if(isset($this->_service[$name]) === false
		|| $this->_service[$name]['monitor'] === 0)
			return(false);
		else if(xivo_issa('unmonitorable',$this->_conf) === true
		&& isset($this->_conf['unmonitorable'][$name]) === true
		&& (bool) $this->_conf['unmonitorable'][$name] === false)
			return(false);

		return(true);
	}

	function is_startable_service($name)
	{
		if(isset($this->_service[$name]) === false
		|| $this->_service[$name]['type'] !== 3
		|| $this->_service[$name]['monitor'] === 2)
			return(false);
		else if($this->_service[$name]['monitor'] === 1
		&& $this->_service[$name]['status'] === 0)
			return(false);
		else if(xivo_issa('startable',$this->_conf) === true
		&& isset($this->_conf['startable'][$name]) === true
		&& (bool) $this->_conf['startable'][$name] === false)
			return(false);

		return(true);
	}

	function is_stoppable_service($name)
	{
		if(isset($this->_service[$name]) === false
		|| $this->_service[$name]['type'] !== 3)
			return(false);
		else if($this->_service[$name]['monitor'] === 1
		&& $this->_service[$name]['status'] !== 0)
			return(false);
		if(xivo_issa('stoppable',$this->_conf) === true
		&& isset($this->_conf['stoppable'][$name]) === true
		&& (bool) $this->_conf['stoppable'][$name] === false)
			return(false);

		return(true);
	}

	function is_restartable_service($name)
	{
		if(isset($this->_service[$name]) === false
		|| $this->_service[$name]['type'] !== 3
		|| $this->_service[$name]['monitor'] === 2)
			return(false);
		else if($this->_service[$name]['monitor'] === 1
		&& $this->_service[$name]['status'] !== 0)
			return(false);
		else if(xivo_issa('restartable',$this->_conf) === true
		&& isset($this->_conf['restartable'][$name]) === true
		&& (bool) $this->_conf['restartable'][$name] === false)
			return(false);

		return(true);
	}

	function enable_monitor($name)
	{
		if($this->is_monitorable($name) === true)
			return($this->_manage_service($name,'monitor'));

		return(false);
	}

	function disable_monitor($name)
	{
		if($this->is_unmonitorable($name) === true)
			return($this->_manage_service($name,'unmonitor'));

		return(false);
	}

	function start_service($name)
	{
		if($this->is_restartable_service($name) === true)
			return($this->_manage_service($name,'start'));

		return(false);
	}

	function restart_service($name)
	{
		if($this->is_restartable_service($name) === true)
			return($this->_manage_service($name,'restart'));

		return(false);
	}

	function stop_service($name)
	{
		if($this->is_stoppable_service($name) === true)
			return($this->_manage_service($name,'stop'));

		return(false);
	}

	function get_typename($type)
	{
		if(isset($this->_types[$type]) === true)
			return($this->_types[$type]);

		return(false);
	}

	function get_statusname($status,$type)
	{
		$name = $not = '';

		if((bool) $status === true)
		{
			ksort($this->_events);

			foreach($this->_events as $key => $value)
			{
				if(($status & $key) !== 0)
					return($value);
			}

			$not = 'not';
		}

		if($name === '' && isset($this->_status[$type]) === true)
			return($not.$this->_status[$type]);

		return(false);
	}

	function get_monitorname($monitor)
	{
		if(isset($this->_monitor[$monitor]) === true)
			return($this->_monitor[$monitor]);

		return(false);
	}

	function recv_info()
	{
		if($this->_conf === false
		|| ($data = $this->_curl->load($this->_statusurl,$this->_option)) === false
		|| ($infos = xivo_json::decode($data,true)) === false
		|| is_array($infos) === false
		|| empty($infos) === true)
		{
			$this->_curl->close();
			return(false);
		}

		if(isset($infos['monit']) === true && xivo_issa('server',$infos['monit']) === true)
			$this->_monit = $infos['monit']['server'];

		if(xivo_issa('service',$infos) === false
		|| ($nb = count($infos['service'])) === 0)
		{
			$this->_curl->close();
			return(true);
		}

		$this->_service		= array();
		$this->_group		= array();
		$this->_device		= array();
		$this->_directory	= array();
		$this->_file		= array();
		$this->_process		= array();
		$this->_system		= array();
		$this->_remotehost	= array();
		$this->_fifo		= array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($typename = $this->get_typename($infos['service'][$i]['type'])) === false
			|| ($monitorname = $this->get_monitorname($infos['service'][$i]['monitor'])) === false)
				continue;

			$name = $infos['service'][$i]['name'];

			$this->_service[$name] = $infos['service'][$i];
			$ref = &$this->_service[$name];
			$ref['typename'] = $typename;
			$ref['monitorname'] = $monitorname;

			if($ref['monitor'] === 1)
				$ref['state'] = $this->get_statusname($ref['status'],$ref['type']);
			else
				$ref['state'] = $monitorname;

			$ref['startable'] = $this->is_startable_service($name);
			$ref['stoppable'] = $this->is_stoppable_service($name);
			$ref['restartable'] = $this->is_restartable_service($name);
			$ref['monitorable'] = $this->is_monitorable($name);
			$ref['unmonitorable'] = $this->is_unmonitorable($name);

			if($ref['type'] === 5)
			{
				$this->_system = $ref;
				continue;
			}

			if(xivo_issa('service',$this->_conf) === true
			&& isset($this->_conf['service'][$name]) === true
			&& (bool) $this->_conf['service'][$name] === false)
			{
				unset($this->_service[$name]);
				continue;
			}

			switch($typename)
			{
				case 'Device':
					$this->_device[$name] = $ref;
					break;
				case 'Directory':
					$this->_directory[$name] = $ref;
					break;
				case 'File':
					$this->_file[$name] = $ref;
					break;
				case 'Process':
					$this->_process[$name] = $ref;
					break;
				case 'Remote Host':
					$this->_remotehost[$name] = $ref;
					break;
				case 'Fifo':
					$this->_fifo[$name] = $ref;
					break;
			}

			if($ref['group'] !== '')
				$grpname = $group = $ref['group'];
			else
			{
				$grpname = 'undefined';
				$group = '__undefined__';
			}

			if(xivo_issa('group',$this->_conf) === true
			&& isset($this->_conf['group'][$grpname]) === true
			&& (bool) $this->_conf['group'][$grpname] === false)
				continue;

			if(isset($this->_group[$group]) === false)
				$this->_group[$group] = array();

			$this->_group[$group][] = $ref;
		}

		$this->_curl->close();
		return(true);
	}
}

?>
