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

class dwho_location
{
	var $_action_misc	= array();
	var $_action_extension	= 'php';

	function dwho_location($options=array(),$action_misc=null)
	{
		$options = (array) $options;

		if(isset($options['action']) === false
		|| ($action = dwho_file::is_d_r($options['action'])) === false)
			trigger_error('Missing or invalid action option in '.__CLASS__,E_USER_ERROR);

		if(isset($options['script']) === false
		|| ($script = dwho_file::is_d_r($options['script'])) === false)
			trigger_error('Missing or invalid script option in '.__CLASS__,E_USER_ERROR);

		if(isset($options['root_url']) === true && is_string($options['root_url']) === true)
			$root_url = rtrim($options['root_url'],'/');
		else
			$root_url = '';

		define('DWHO_LOCATION_ACTION',$action);
		define('DWHO_LOCATION_SCRIPT',$script);
		define('DWHO_LOCATION_ROOT_URL',$root_url);

		if(is_array($action_misc) === true)
			$this->_action_misc = $action_misc;
	}

	function get_current_location($ret=true)
	{
		$root_url = DWHO_LOCATION_ROOT_URL;

		if(($len = strlen($root_url)) === 0)
			return($_SERVER['SCRIPT_NAME']);
		else if(strncmp($root_url.'/',$_SERVER['SCRIPT_NAME'],$len + 1) === 0)
			return(substr($_SERVER['SCRIPT_NAME'],$len));

		return(((bool) $ref === true ? $_SERVER['SCRIPT_NAME'] : false));
	}

	function cmp_to_current_location($location)
	{
		if(is_scalar($location) === false)
			return(false);
		else if(DWHO_LOCATION_ROOT_URL === '')
			$url = $location;
		else
			$url = DWHO_LOCATION_ROOT_URL.'/'.ltrim($location,'/');

		return((strcmp($url,$_SERVER['SCRIPT_NAME']) === 0));
	}

	function get_action_path($section=null,$nb=0,$path='',$ext='')
	{
		$nb = dwho_uint($nb,20);
		$path = (string) $path;

		$search = array('@/+@','@^/@','@/$@','@\.\.@');
		$replace = array(DWHO_SEP_DIR,'','','');

		$section = preg_replace($search,$replace,$section);

		if($path === '' && isset($_SERVER['PATH_INFO']) === true)
			$path = $_SERVER['PATH_INFO'];

		if($path !== '')
		{
			$pathcleaned = preg_replace($search,$replace,$path);

			if(($exp = explode(DWHO_SEP_DIR,$pathcleaned,$nb + 1)) === false)
				return(false);

			$path = '';

			$cnt = count($exp);

			for($i = 0;$i < $cnt && $i < $nb;$i++)
			{
				if(isset($exp[$i]{0}) === false || $exp[$i]{0} === '_' || $exp[$i]{0} === '.')
					return(false);

				$path .= DWHO_SEP_DIR.$exp[$i];
			}

			$path = substr($path,1);
		}

		define('DWHO_LOCATION_ACTION_PATH',$path);
		define('DWHO_LOCATION_ACTION_SECTION',$section);

		$filepath = trim($section.DWHO_SEP_DIR.$path,DWHO_SEP_DIR);
		$newpath = $this->_apply_action_misc($filepath);

		if(is_string($newpath) === true)
			$filepath = $newpath;

		return(dwho_file::is_f_r(DWHO_LOCATION_ACTION.
					 DWHO_SEP_DIR.
					 $filepath.
					 '.'.$this->_action_extension));
	}

	function _apply_action_misc($action_path)
	{
		if(dwho_issa($action_path,$this->_action_misc) === false)
			return(false);

		$list = $this->_action_misc[$action_path];

		if(dwho_issa('define',$list) === true)
		{
			foreach($list['define'] as $key => $value)
			{
				if(defined($key) === false && is_scalar($value) === true)
					define($key,$value);
			}
		}

		if(isset($list['path'],$list['path']{0}) === true)
			return($list['path']);

		return(true);
	}
}

?>
