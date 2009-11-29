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

dwho::load_class('dwho_query');
dwho::load_class('dwho_uri');

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'tpl','module.inc'));

class dwho_tpl_url extends dwho_tpl_module
{
	var $_tpl	= null;
	var $_query	= null;
	var $_uri	= null;

	function dwho_tpl_url(&$tpl)
	{
		$this->_tpl		= &$tpl;
		$this->_query		= &dwho_gct::set_get(new dwho_query());
		$this->_uri		= new dwho_uri();
		$this->_separator	= '&amp;';
	}

	function get_separator($sep=null)
	{
		if(dwho_has_len($sep) === true)
			return($sep);

		return($this->_separator);
	}

	function _build_query($query=null,$encode=true,$sep=null,$sid=true)
	{
		$sid = (bool) $sid;
		$sep = $this->get_separator($sep);

		if(is_array($query) === true)
			$query = $this->_query->build_query_str($query,$encode,$sep);
		else
			$query = strval($query);

		if(isset($query{0}) === true)
			return('?'.$query.($sid === true ? $sep.DWHO_SESS_STR : ''));
		else if($sid === true)
			return('?'.DWHO_SESS_STR);

		return('');
	}

	function href($url=null,$query=null,$absolute=false,$sep=null,$sid=true,$internal=true,$encode=true)
	{
		$url = $this->url($url,$absolute,$internal);
		$sep = $this->get_separator($sep);

		if(substr($url,0,1) !== '#')
			$url .= $this->_build_query($query,$encode,$sep,$sid);

		if((bool) $encode === false)
			return($url);

		return($this->_uri->build_uri($this->_uri->parse_uri($url,true,$sep),true,null,$sep));
	}

	function href_html($content,
			   $url=null,
			   $query=null,
			   $opt=null,
			   $title=null,
			   $absolute=false,
			   $sep='&amp;',
			   $sid=true,
			   $internal=true,
			   $encode=true)
	{
		$url = $this->href($url,$query,$absolute,$sep,$sid,$internal,$encode);

		return($this->_mk_href_html($content,$url,$opt,$title));
	}

	function href_html_std($content,$url=null,$query=null,$opt=null,$title=null,$encode=true)
	{
		return($this->href_html($content,$url,$query,$opt,$title,false,null,false,false,$encode));
	}

	function _mk_href_html($content,$url=null,$opt=null,$title=null)
	{
		$content	= (string) $content;
		$opt		= (string) $opt;

		if($title !== null)
			$title = (string) $title;
		else
			$title = $content;

		$title = isset($title{0}) === true ? ' title="'.trim(dwho_alttitle($title)).'"' : '';

		return('<a href="'.$url.'"'.(isset($opt{0}) === true ? ' '.$opt : '').$title.'>'.$content.'</a>');
	}

	function img($url,$i18n=false,$internal=true)
	{
		$url = (string) $url;

		if(isset($url{0}) === false)
			return('');
		else if($url{0} === '/' || (bool) $internal === false)
			return($url);
		else if((bool) $i18n === false)
			return(DWHO_LOCATION_ROOT_URL.'/'.$url);

		$user = &$this->_get_module('user');

		return(DWHO_LOCATION_ROOT_URL.'/img/i18n/'.$user->get_info('language').'/'.$url);
	}

	function img_html($url,$alt=null,$opt=null,$query=null,$i18n=false,$internal=true)
	{
		$opt = dwho_has_len($opt) === true ? ' '.$opt : '';
		$alt = trim(dwho_alttitle($alt));

		return('<img src="'.$this->img($url,$i18n,$internal).'"'.$opt.' alt="'.$alt.'" />');
	}

	function pager($nb,
		       $page,
		       $prev=false,
		       $next=false,
		       $url=null,
		       $query=null,
		       $opt=null,
		       $absolute=false,
		       $sep=null,
		       $sid=true,
		       $internal=true,
		       $encode=true)
	{
		$r	= '';
		$nb	= dwho_uint($nb);
		$page	= dwho_uint($page,1);
		$sep	= $this->get_separator($sep);

		if($nb < 2)
			return($r);

		if($nb < 7)
			$a = array_fill(1,$nb,0);
		else
		{
			$a = array(1	=> 0,
				   2	=> 0,
				   3	=> 0);

			$page1 = $page - 1;
			$page2 = $page + 1;

			if($nb > 6)
			{
				$nb_page1 = $nb - 1;
				$nb_page2 = $nb - 2;
				$a[$nb] = 0;
				$a[$nb_page1] = 0;
				$a[$nb_page2] = 0;
			}

			$a[$page] = 0;

			if($page1 > 0)
				$a[$page1] = 0;
			if($page2 <= $nb)
				$a[$page2] = 0;
			ksort($a);
		}

		if(($query = $this->_build_query($query,$encode,$sep,$sid)) === '')
			$query = '?';
		else
			$query .= $sep;

		$url = $this->url($url,$absolute,$internal).$query.'page=';

		if((bool) $prev === true)
			$r .= $this->_mk_href_html($this->bbf('page_previous'),
						   $url.($page - 1),
						   $opt,
						   $this->bbf('page_previous')).' - ';

		reset($a);

		while(list($k) = each($a))
		{
			$pagek = $k === $page ? '<b>'.$k.'</b>' : $k;

			$slt = $this->_mk_href_html($pagek,
						    $url.$k,
						    $opt,
						    $this->bbf('page_number',$k));

			if($k > 4 && isset($a[$k - 1]) === false)
				$r .= ' ... '.$slt.', ';
			else if($k > 4 && $k < $nb-5 && isset($a[$k - 1]) === false)
				$r .= $slt.' ... ';
			else if(isset($a[$k + 1]) === true)
				$r .= $slt.', ';
			else
				$r .= $slt;
		}

		if((bool) $next === true)
			$r .= ' - '.$this->_mk_href_html($this->bbf('page_next'),
							 $url.($page + 1),
							 $opt,
							 $this->bbf('page_next'));

		return($r);
	}
}

?>
