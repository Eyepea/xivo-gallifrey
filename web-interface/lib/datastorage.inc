<?php

class xivo_datastorage
{
	function factory($type,$param=array())
	{
		$type = strtolower((string) $type);

		if(xivo_datastorage::is_valid($type) === false)
			trigger_error('Invalid datastorage type');

		if(xivo_datastorage::chk_extension_exists($type) === true && xivo_datastorage::chk_extension_loaded($type,true) === false)
		       trigger_error('Datastorage '.$type.' extension is not loaded',E_USER_ERROR);	

		$name = 'xivo_datastorage_'.$type;

		if(xivo::load_class($name,'datastorage',$type) === false)
			return(false);

		return(new $name($param));
	}

	function is_valid($type)
	{
		$type = (string) $type;

		$datastorage = &xivo_gat::get('datastorage');

		return(isset($datastorage[$type]));
	}

	function get_extension_name($type)
	{
		$r = false;

		$type = (string) $type;

		$datastorage = &xivo_gat::get('datastorage');

		if(xivo_datastorage::chk_extension_exists($type) === true)
			$r = $datastorage[$type]['extension'];

		return($r);
	}

	function chk_extension_loaded($type,$load=false)
	{
		$r = false;

		$type = (string) $type;
		$load = (bool) $load;

		$datastorage = &xivo_gat::get('datastorage');

		if(xivo_datastorage::chk_extension_exists($type) === true)
		{
			if($load === true)
				xivo::load_extension($datastorage[$type]['extension']);

			$r = xivo::chk_extension_loaded($datastorage[$type]['extension']);
		}

		return($r);
	}

	function chk_extension_exists($type)
	{
		$r = false;

		$type = (string) $type;

		$datastorage = &xivo_gat::get('datastorage');

		if(xivo_datastorage::is_valid($type) === true && xivo_ak('extension',$datastorage[$type]) === true)
			$r = true;

		return($r);
	}

	function chk_link($type)
	{
		$r = false;

		$type = (string) $type;

		$datastorage = &xivo_gat::get('datastorage');

		if(xivo_datastorage::is_valid($type) === true && xivo_issa('link',$datastorage[$type],true) === true)
			$r = true;

		return($r);
	}
}

?>
