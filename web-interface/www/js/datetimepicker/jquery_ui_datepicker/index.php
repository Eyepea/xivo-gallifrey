<?php


	$lang		= 'de';
	$field_id 	= 'datepicker';
	$timepr		= TRUE;
	        		
	if (isset($params['caller']['id'])) {
		$field_id = $params['caller']['id'];
	}


	
	if (!$already_loaded) {
		$_output[] = "<link rel=\"stylesheet\" type=\"text/css\" href=\"".$widgetpath_web."smothness/jquery_ui_datepicker.css\">";
		$_output[] = '<script type="text/javascript" src="'.$widgetpath_web.'i18n/ui.datepicker-'.$lang.'.js"></script>';
	
		if ($timepr) {
			$_output[] = "<link rel=\"stylesheet\" type=\"text/css\" href=\"".$widgetpath_web."timepicker_plug/css/style.css\">";
			$_output[] = '<script type="text/javascript" src="'.$widgetpath_web.'timepicker_plug/timepicker.js"></script>';		
				
		}
	}	
	
	
	$_output[] = "<script type=\"text/javascript\">";
	$_output[] = '/* <![CDATA[ */';
	$_output[] = '	$(function() {';
	$_output[] = '	';
	$_output[] = '			$(\'#'.$field_id.'\').datetime();';
	$_output[] = '			$(\'#field2\').datetime();';
	$_output[] = '	';
	$_output[] = '});';
	$_output[] = '/* ]]> */';
	$_output[] = "</script>";
	
	$framework = 'jquery13';   
?>

