<?php

class xivo_tpl_image
{
	function make($url,$alt='',$opt='')
	{
		$r = '';

		$url = (string) $url;
		$alt = (string) $alt;
		$opt = (string) $opt;

		if(isset($url{0}) === false)
			return($r);

		$url = $url{0} !== '/' ? '/i/i18n/'.XIVO_LC_META.'/'.$url : $url;

		$r = '<img src="'.$url.'"'.(isset($opt{0}) === true ? ' '.$opt : '').' alt="'.xivo_alttitle($alt).'" />';

		return($r);
	}

	function get($url)
	{
		$r = '';

		$url = (string) $url;

		if(isset($url{0}) === false)
			return($r);

		$r = $url{0} !== '/' ? '/i/i18n/'.XIVO_LC_META.'/'.$url : $url;

		return($r);
	}
}

?>
