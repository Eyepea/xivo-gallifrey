<?php

$vendor = $this->get_var('vendor');

$param = array();

switch($vendor)
{
	case 'thomson':
		$vendor = 'genericxml';
		$param['tagmenu'] = 'ThomsonPhoneMenu';
		$param['tagdirectory'] = 'ThomsonPhoneBook';
		$param['taginput'] = '';
		break;
	case 'snom':
		$vendor = 'genericxml';
		$param['tagmenu'] = 'SnomIPPhoneMenu';
		$param['tagdirectory'] = 'SnomIPPhoneDirectory';
		$param['taginput'] = 'SnomIPPhoneInput';
		break;
	default:
		xivo_die('Error/Invalid Vendor and User-Agent');
}

$this->file_include($this->get_var('path').'/'.$vendor.'/'.$this->get_var('act'),$param);

?>
