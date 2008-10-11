<?php

$xmlphone = &$this->get_module('xmlphone',
			       array('vendor'	=> $this->get_var('vendor')));

if(($vendor = $directory = $xmlphone->get_vendor()) === false)
	xivo_die('Error/Invalid Vendor and User-Agent');

header($xmlphone->get_header_contenttype());

$param = array();

switch($vendor)
{
	case 'thomson':
	case 'snom':
		$directory = 'genericxml';
		break;
}

$this->file_include($this->get_var('path').'/'.$directory.'/'.$this->get_var('act'),
		    $param);

?>
