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


dwho::load_class('dwho_network');
dwho::load_class('dwho_http');

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_skill extends xivo_application_asterisk_abstract
{
	var $_dso				= null;
	var $_skillcat	= false;
	var $_skill			= false;

	function xivo_application_service_asterisk_skill(&$service,&$dso,$param=array(),$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = array();
		if($this->load_init($param) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		$this->_load_config(dirname(__FILE__));

/*
		if(($size = $this->_verify_can_upload_file()) === false)
			die();

		$this->_conf['import']['file']['size'] = $size;
*/

		return(true);
	}

	function load_init($param=array())
	{
		$this->_reset_info();
		$this->_reset_params();
		$this->_reset();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		$this->_skillcat		= &$this->_service->get_module('skillcat');
		$this->_skill				= &$this->_service->get_module('skill');

		return(true);
	}

	/** Return all users
	**/
	function get_all()
	{
		$rs = $this->_dso->get_all();
		return ($rs);
	}

	function set_n_add($arr)
	{
		$this->_reset();

		$name 		= $arr['skill']['name'];

		/*if(is_array($arr) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid data');
			return(false);
		}*/
		$values		= $arr['skill']['values'];//array('values' => $arr['skill']['values']);
		/*
		if(is_array($arr) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid data');
			return(false);
		}
		*/

		//echo "add: $name / $values. ";
		// ret === inserted ID
		$catid =  $this->_skillcat->add(array('id' => null, 'name' => $name));
		//echo "ret= $catid. ";
		//echo "ret is none: " . ($catid == null?"YES":"NO") . ". ";
		if($catid != null)
		{

			//print_r($values[0]);
			foreach($values as $k => $v)
			{
				//print_r($v);
				$value = array(
					'id' 					=> null, 
					'catid' 			=> $catid,
					'name'				=> $v[0],
					'description'	=> $v[1],
					'printscreen'	=> $v[2]
				);

				$skid = $this->_skill->add($value);
			}
		}

		$ret = $this->_skill->add($values);

		//echo "errs= " . $this->get_errnb() . ". ";
		return(($this->get_errnb() < 1));
	}

	//function get($id,$disable=null,$internal=false,$where=false,$nocomponents=null)
	function get($id)
	{
		$this->_info = $this->_origin = array();
		
		return $this->_dso->get($id);
	}
}

?>
