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

class dwho_tpl_module
{
	function load_init($param=null)
	{
		return(null);
	}

	function bbf_args($name,$args,$namespace='')
	{
		return($this->_tpl->bbf_args($name,$args,$namespace));
	}

	function bbf($name,$args=array(),$namespace='')
	{
		return($this->_tpl->bbf($name,$args,$namespace));
	}

	function load_i18n_file($file,$namespace='')
	{
		return($this->_tpl->load_i18n_file($file,$namespace));
	}

	function get_path()
	{
		return($this->_tpl->get_path());
	}

	function set_option($name,$value)
	{
		return($this->_tpl->set_option($name,$value));
	}

	function get_option($name)
	{
		return($this->_tpl->get_option($name));
	}

	function get_file_extension($ext)
	{
		return($this->_tpl->get_file_extension($ext));
	}

	function set_var($name,$value)
	{
		return($this->_tpl->set_var($name,$value));
	}

	function set_vars(&$a)
	{
		return($this->_tpl->set_vars(&$a));
	}

	function get_var()
	{
		$args = func_get_args();
		return(call_user_func_array(array(&$this->_tpl,'get_var'),$args));
	}

	function get_var_default()
	{
		$args = func_get_args();
		return(call_user_func_array(array(&$this->_tpl,'get_var_default'),$args));
	}

	function set_file($name,$value,$preload_i18n=false)
	{
		return($this->_tpl->set_file($name,$value,$preload_i18n));
	}

	function get_file($name)
	{
		return($this->_tpl->get_file($name));
	}

	function file_include($file,$arr=array(),$preload_i18n=false,$ext='')
	{
		return($this->_tpl->file_include($file,$arr,$preload_i18n,$ext));
	}

	function file_time($file)
	{
		return($this->_tpl->file_time($file));
	}

	function url($url,$absolute=false,$internal=true,$extern=false)
	{
		return($this->_tpl->url($url,$absolute,$internal,$extern));
	}

	function &_get_module($name)
	{
		return(($ref = &$this->_tpl->get_module($name)));
	}
}

?>
