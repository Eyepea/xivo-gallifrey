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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'tpl','module.inc'));

class dwho_tpl_url extends dwho_tpl_module
{
	var $_tpl	= null;

	function dwho_tpl_url(&$tpl)
	{
		$this->_tpl = &$tpl;
	}

	function href($url='',$param='',$absolute=false,$sep=null,$sid=true,$internal=true)
	{
		$url = $this->url($url,$absolute,$internal);

		if(isset($url{0}) === true && $url{0} === '#')
			return($url);

		$url .= $this->_mk_param($param,$sep,$sid);

		return($url);
	}

	function _mk_param($param='',$sep=null,$sid=true)
	{
		$sid = (bool) $sid;
		$sep = $sep === null ? '&amp;' : (string) $sep;

		if(is_array($param) === true)
			$param = dwho_query::mk_query_str($param,$sep);
		else
			$param = (string) $param;

		if(isset($param{0}) === true)
			return('?'.$param.($sid === true ? $sep.DWHO_SESS_STR : ''));
		else if($sid === true)
			return('?'.DWHO_SESS_STR);

		return('');
	}

	function href_html($content,$url='',$param='',$opt='',$title=null,$absolute=false,$sep='&amp;',$sid=true,$internal=true)
	{
		$url = $this->href($url,$param,$absolute,$sep,$sid,$internal);

		return($this->_mk_href_html($content,$url,$opt,$title));
	}

	function _mk_href_html($content,$url='',$opt='',$title=null)
	{
		$content = (string) $content;
		$opt = (string) $opt;

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
			return($this->get_option('script_root').'/'.$url);

		$usr = &$this->_get_module('user');
		return($this->get_option('script_root').'/img/i18n/'.$usr->get_info('language').'/'.$url);
	}

	function img_html($url,$alt='',$opt='',$i18n=false,$internal=true)
	{
		$opt = (string) $opt;

		$url = $this->img($url,$i18n,$internal);

		return('<img src="'.$url.'"'.(isset($opt{0}) === true ? ' '.$opt : '').' alt="'.trim(dwho_alttitle($alt)).'" />');
	}

	function pager($nb,$page,$prev=false,$next=false,$url='',$param='',$opt='',$absolute=false,$sep=null,$sid=true,$internal=true)
	{
		$r = '';

		$nb = dwho_uint($nb);
		$page = dwho_uint($page,1);
		$sep = $sep === null ? '&amp;' : (string) $sep;

		if($nb < 2)
			return($r);

		if($nb < 7)
			$a = array_fill(1,$nb,0);
		else
		{
			$a = array(1 => 0,2 => 0,3 => 0);
			$page1 = $page-1;
			$page2 = $page+1;

			if($nb > 6)
			{
				$nb_page1 = $nb-1;
				$nb_page2 = $nb-2;
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

		if(($param = $this->_mk_param($param,$sep,$sid)) === '')
			$param = '?';
		else
			$param .= $sep;

		$url = $this->url($url,$absolute,$internal).$param.'page=';

		if((bool) $prev === true)
			$r .= $this->_mk_href_html($this->bbf('page_previous'),
						   $url.($page - 1),
						   $opt,
						   $this->bbf('page_previous')).' - ';

		reset($a);

		while(list($k) = each($a))
		{
			$pagek = $k === $page ? '<b>'.$k.'</b>' : $k;

			$slt = $this->_mk_href_html($pagek,$url.$k,$opt,$this->bbf('page_number',$k));

			if($k > 4 && isset($a[$k-1]) === false)
				$r .= ' ... '.$slt.', ';
			else if($k > 4 && $k < $nb-5 && isset($a[$k-1]) === false)
				$r .= $slt.' ... ';
			else if(isset($a[$k+1]) === true)
				$r .= $slt.', ';
			else
				$r .= $slt;
		}

		if((bool) $next === true)
			$r .= ' - '.$this->_mk_href_html($this->bbf('page_next'),$url.($page + 1),$opt,$this->bbf('page_next'));

		return($r);
	}
}

?>
