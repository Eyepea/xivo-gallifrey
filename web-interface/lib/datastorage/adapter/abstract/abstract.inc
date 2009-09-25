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

class dwho_datastorage_adapter_abstract
{
	var $_param	= array();

	var $_error	= '';
	var $_errno	= 0;
	var $_quote	= false;
	var $_link	= null;
	var $_result	= false;
	var $_debug	= false;
	var $_free	= false;
	var $_from	= '';
	var $_table	= array();
	var $_column	= array();
	var $_colall	= array();
	var $_where	= '';
	var $_aggwhere	= array();
	var $_order	= array();
	var $_limit	= array();

	function set_param($name,$value)
	{
		$name = (string) $name;

		$this->_param[$name] = $value;
	}

	function get_param($name)
	{
		$name = (string) $name;

		if(isset($this->_param[$name]) === true)
			return($this->_param[$name]);

		return(null);
	}

	function set_debug($debug=false)
	{
		$this->_debug = (bool) $debug;
	}

	function get_type()
	{
		return($this->_type);
	}

	function get_meta()
	{
		return($this->_meta);
	}

	function halt($str='')
	{
		$str = (string) $str;

		if(($this->_errno = $this->errno()) === 0)
			$this->_errno = 'unknown';

		if(($this->_error = $this->error()) === '')
			$this->_error = 'unknown';

		if($str !== '')
			$str .= ' - ';

		if($this->_debug === true && empty($this->_query) === false)
			$str .= 'Query: '.$this->_query.' - ';

		trigger_error($str.'Error: '.$this->_error.' - Errno: '.$this->_errno,E_USER_ERROR);
	}

	function is_open()
	{
		return(is_resource($this->_link));
	}

	function is_free()
	{
		return((bool) $this->_free);
	}

	function free()
	{
		return(false);
	}

	function quote_identifier($name)
	{
		return((dwho_has_len($name) === true ? $name : ''));
	}

	function escape($str)
	{
		return($this->escape_string(strval($str)));
	}

	function escape_string($str)
	{
		return((string) $str);
	}

	function reset_all()
	{
		$this->_table = array();
		$this->reset_from();
		$this->reset_column();
		$this->reset_where();
		$this->reset_order();
		$this->reset_limit();
	}

	function reset_column()
	{
		$this->_column = $this->_colall = array();
	}

	function get_column()
	{
		if(empty($this->_column) === false)
			return($this->_column);

		return(null);
	}

	function reset_from()
	{
		$this->_from = '';
	}

	function from($table,$column='*')
	{
		if($this->_mk_table($table) === false)
			return(false);

		return($this->_mk_column($column));
	}

	function get_from()
	{
		if($this->_from !== '')
			return($this->_from);

		return(null);
	}

	function new_select($table,$column='*')
	{
		$this->reset_all();

		return($this->from($table,$column));
	}

	function get_select()
	{
		if(($r = $this->_mk_select()) !== false)
			return($r);

		return(null);
	}

	function reset_where()
	{
		$this->_where = '';
		$this->_aggwhere = array();
	}

	function get_where()
	{
		if(dwho_has_len($this->_where) === false)
			return(null);

		return($this->_where);
	}

	function where($cond,$val='',$condand=true)
	{
		return($this->andwhere($cond,$val,$condand));
	}

	function andwhere($cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,true,false));
	}

	function andwherenot($cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,true,true));
	}

	function orwhere($cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,false,false));
	}

	function orwherenot($cond,$val='',$condand=true)
	{
		return($this->_set_where($cond,$val,$condand,false,true));
	}

	function wherenull($column,$table='')
	{
		return($this->andwherenull($column,$table));
	}

	function andwherenull($column,$table='')
	{
		return($this->_set_wherenull($column,$table,true,false));
	}

	function andwherenotnull($column,$table='')
	{
		return($this->_set_wherenull($column,$table,true,true));
	}

	function orwherenull($column,$table='')
	{
		return($this->_set_wherenull($column,$table,false,false));
	}

	function orwherenotnull($column,$table='')
	{
		return($this->_set_wherenull($column,$table,false,true));
	}

	function wherebetween($column,$beg,$end,$table='')
	{
		return($this->andwherebetween($column,$beg,$end,$table));
	}

	function andwherebetween($column,$beg,$end,$table='')
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,true,false));
	}

	function andwherenotbetween($column,$beg,$end,$table='')
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,true,true));
	}

	function orwherebetween($column,$beg,$end,$table='')
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,false,false));
	}

	function orwherenotbetween($column,$beg,$end,$table='')
	{
		return($this->_set_wherebetween($column,$beg,$end,$table,false,true));
	}

	function _set_wherebetweencolumn($val,$beg,$end,$table='',$and=true,$not=false)
	{
		return($this->_set_wherebetween($val,$beg,$end,$table,$and,$not));
	}

	function wherebetweencolumn($val,$beg,$end,$table='')
	{
		return($this->andwherebetweencolumn($val,$beg,$end,$table));
	}

	function andwherebetweencolumn($val,$beg,$end,$table='')
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,true,false));
	}

	function andwherenotbetweencolumn($val,$beg,$end,$table='')
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,true,true));
	}

	function orwherebetweencolumn($val,$beg,$end,$table='')
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,false,false));
	}

	function orwherenotbetweencolumn($val,$beg,$end,$table='')
	{
		return($this->_set_wherebetweencolumn($val,$beg,$end,$table,false,true));
	}

	function wherein($column,$list,$table='',$keys=false)
	{
		return($this->andwherein($column,$list,$table,$keys));
	}

	function andwherein($column,$list,$table='',$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,true,false));
	}

	function andwherenotin($column,$list,$table='',$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,true,true));
	}

	function orwherein($column,$list,$table='',$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,false,false));
	}

	function orwherenotin($column,$list,$table='',$keys=false)
	{
		return($this->_set_wherein($column,$list,$table,$keys,false,true));
	}

	function wherelike($column,$pattern,$format,$condand=true,$table='')
	{
		return($this->andwherelike($column,$pattern,$format,$condand,$table));
	}

	function andwherelike($column,$pattern,$format,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,$condand,true,false));
	}

	function andwherenotlike($column,$pattern,$format,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,$condand,true,true));
	}

	function orwherelike($column,$pattern,$format,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,$condand,false,false));
	}

	function orwherenotlike($column,$pattern,$format,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,$format,$condand,false,true));
	}

	function whereexact($column,$pattern,$condand=true,$table='')
	{
		return($this->andwhereexact($column,$pattern,$condand,$table));
	}

	function andwhereexact($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',$condand,true,false));
	}

	function andwherenotexact($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',$condand,true,true));
	}

	function orwhereexact($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',$condand,false,false));
	}

	function orwherenotexact($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'exact',$condand,false,true));
	}

	function wherebegin($column,$pattern,$condand=true,$table='')
	{
		return($this->andwherebegin($column,$pattern,$condand,$table));
	}

	function andwherebegin($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',$condand,true,false));
	}

	function andwherenotbegin($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',$condand,true,true));
	}

	function orwherebegin($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',$condand,false,false));
	}

	function orwherenotbegin($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'begin',$condand,false,true));
	}

	function wherecontain($column,$pattern,$condand=true,$table='')
	{
		return($this->andwherecontain($column,$pattern,$condand,$table));
	}

	function andwherecontain($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',$condand,true,false));
	}

	function andwherenotcontain($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',$condand,true,true));
	}

	function orwherecontain($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',$condand,false,false));
	}

	function orwherenotcontain($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'contain',$condand,false,true));
	}

	function whereend($column,$pattern,$condand=true,$table='')
	{
		return($this->andwhereend($column,$pattern,$condand,$table));
	}

	function andwhereend($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',$condand,true,false));
	}

	function andwherenotend($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',$condand,true,true));
	}

	function orwhereend($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',$condand,false,false));
	}

	function orwherenotend($column,$pattern,$condand=true,$table='')
	{
		return($this->_set_wherelike($column,$pattern,$table,'end',$condand,false,true));
	}

	function new_order($column,$sort='',$table='')
	{
		$this->reset_order();

		if(($r = $this->_set_order($column,$sort,$table)) === false)
			return(false);

		$this->_order = $r;

		return(true);
	}

	function add_order($column,$sort='',$table='')
	{
		if(($r = $this->_set_order($column,$sort,$table)) === false)
			return(false);

		$this->_order = array_merge($this->_order,$r);

		return(true);
	}

	function reset_order()
	{
		$this->_order = array();
	}

	function get_order()
	{
		if(($r = $this->_mk_order()) !== false)
			return($r);

		return(null);
	}

	function _set_sort($sort='')
	{
		$sort = strtoupper($sort);

		switch($sort)
		{
			case 'D':
			case 'DESC':
			case SORT_DESC:
				return(SORT_DESC);
		}

		return(SORT_ASC);
	}

	function limit($offset,$line='')
	{
		$this->reset_limit();

		if(($r = $this->_set_limit($offset,$line)) === false)
			return(false);

		$this->_limit = $r;

		return(true);
	}

	function reset_limit()
	{
		$this->_limit = array();
	}

	function get_limit()
	{
		if(($r = $this->_mk_limit()) !== false)
			return($r);

		return(null);
	}

	function _set_limit($offset,$line='')
	{
		$r = array();

		if(is_array($offset) === true && isset($offset[0]) === true)
		{
			if(isset($offset[1]) === true)
				$line = $offset[1];

			$offset = $offset[0];
		}

		$offset = (string) $offset;
		$line = (string) $line;

		if($offset === '')
			return(false);

		$r[0] = dwho_uint($offset);

		if($line !== '')
			$r[1] = dwho_uint($line);

		return($r);
	}
}

?>
