<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module.inc');

class xivo_tpl_url extends xivo_tpl_module
{
	var $_tpl	= null;

	function xivo_tpl_url(&$tpl)
	{
		$this->_tpl = &$tpl;
	}

	function href($url='',$param='',$absolute=false,$sep=null,$sid=true,$pre='',$key='')
	{
		$url = $this->url($url,$absolute);

		if(isset($url{0}) === true && $url{0} === '#')
			return($url);

		$url .= $this->_mk_param($param,$sep,$sid,$pre,$key);

		return($url);
	}

	function _mk_param($param='',$sep=null,$sid=true,$pre='',$key='')
	{
		$sid = (bool) $sid;
		$sep = $sep === null ? '&amp;' : (string) $sep;

		if(is_array($param) === true)
			$param = xivo_query::mk_query_str($param,$pre,$key,$sep);
		else
			$param = (string) $param;

		if(isset($param{0}) === true)
			return('?'.$param.($sid === true ? $sep.XIVO_SESS_STR : ''));
		else if($sid === true)
			return('?'.XIVO_SESS_STR);

		return('');
	}

	function href_html($content,$url='',$param='',$opt='',$title=null,$absolute=false,$sep='&amp;',$sid=true,$pre='',$key='')
	{
		$url = $this->href($url,$param,$absolute,$sep,$sid,$pre,$key);

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

		$title = isset($title{0}) === true ? ' title="'.xivo_alttitle($title).'"' : '';

		return('<a href="'.$url.'"'.(isset($opt{0}) === true ? ' '.$opt : '').$title.'>'.$content.'</a>');
	}

	function img($url,$i18n=false)
	{
		$url = (string) $url;

		if(isset($url{0}) === false)
			return('');
		else if($url{0} === '/')
			return($url);
		else if((bool) $i18n === false)
			return($this->get_option('script_root').'/'.$url);

		$usr = &$this->_get_module('user');
		return($this->get_option('script_root').'/img/i18n/'.$usr->get_info('language').'/'.$url);
	}

	function img_html($url,$alt='',$opt='',$i18n=false)
	{
		$opt = (string) $opt;

		$url = $this->img($url,$i18n);

		return('<img src="'.$url.'"'.(isset($opt{0}) === true ? ' '.$opt : '').' alt="'.xivo_alttitle($alt).'" />');
	}

	function pager($nb,$page,$prev=false,$next=false,$url='',$param='',$opt='',$absolute=false,$sep=null,$sid=true,$pre='',$key='')
	{
		$r = '';

		$nb = xivo_uint($nb);
		$page = xivo_uint($page,1);
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

		if(($param = $this->_mk_param($param,$sep,$sid,$pre,$key)) === '')
			$param = '?';
		else
			$param .= $sep;

		$url = $this->url($url,$absolute).$param.'page=';

		if((bool) $prev === true)
			$r .= $this->_mk_href_html($this->bbf('page_previous'),$url.($page - 1),$opt,$this->bbf('page_previous')).' - ';

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
