<?php

class xivo_datastorage
{
	function factory($type,$param=array())
	{
		$type = strtolower($type);
		$name = 'xivo_datastorage_adapter_'.$type;

		if(xivo_datastorage::is_valid($type) === false)
			trigger_error('Invalid datastorage type',E_USER_ERROR);
		else if(xivo_datastorage::chk_extension_loaded($type,true) === false)
			trigger_error('Datastorage '.$type.' extension is not loaded',E_USER_ERROR);
		else if(xivo::load_class($name,array('datastorage','adapter'),$type) === false)
			return(false);

		return(new $name($param));
	}

	function is_valid($type)
	{
		$type = (string) $type;

		$datastorage = &xivo_gat::get('datastorage');

		return(isset($datastorage[$type]));
	}

	function has_ssl($type)
	{
		$type = (string) $type;

		$datastorage = &xivo_gat::get('datastorage');

		if(xivo_datastorage::is_valid($type) === true
		&& (bool) $datastorage[$type]['ssl'] === true)
			return($type);

		return(xivo_datastorage::is_ssl_scheme($type));
	}

	function is_ssl_scheme($type)
	{
		$type = (string) $type;

		$datastorage = &xivo_gat::get('datastorage');

		if(substr($type,-1) === 's'
		&& ($type = substr($type,0,-1)) !== false
		&& xivo_datastorage::is_valid($type) === true
		&& (bool) $datastorage[$type]['ssl'] === true)
			return($type);

		return(false);
	}

	function set_param_from_uri($str)
	{
		xivo::load_class('xivo_uri');
		$uri = new xivo_uri();

		if(($arr = $uri->parse_uri($str)) === false || isset($arr['scheme']) === false)
			return(false);
		else if(($ssl = xivo_datastorage::is_ssl_scheme($arr['scheme'])) !== false)
			$arr['scheme'] = $ssl;
		else if(xivo_datastorage::is_valid($arr['scheme']) === false)
			return(false);

		$r = array();
		$r['type'] = $arr['scheme'];

		if(isset($arr['authority']) === true)
		{
			if(isset($arr['authority']['user']) === true)
				$r['user'] = $arr['authority']['user'];

			if(isset($arr['authority']['passwd']) === true)
				$r['pass'] = $arr['authority']['passwd'];

			if(isset($arr['authority']['host']) === true)
				$r['host'] = $arr['authority']['host'];

			if(isset($arr['authority']['port']) === true)
				$r['port'] = $arr['authority']['port'];
		}

		if(isset($arr['path']) === true)
		{
			if($arr['path']{0} === '/' && isset($arr['authority']) === true)
				$r['db'] = substr($arr['path'],1);
			else
				$r['db'] = $arr['path'];
		}

		if(isset($arr['query']) === true)
		{
			$nb = count($arr['query']);

			for($i = 0;$i < $nb;$i++)
			{
				if(xivo_haslen($arr['query'][$i],0) === false)
					continue;
				else if(isset($arr['query'][$i][1]) === true)
					$r[$arr['query'][$i][0]] = $arr['query'][$i][1];
				else
					$r[$arr['query'][$i][0]] = '';
			}
		}

		$r['ssl'] = $ssl !== false;

		return($r);
	}

	function get_extension_name($type)
	{
		$type = (string) $type;

		if(xivo_datastorage::chk_extension_exists($type) === false)
			return(false);

		$datastorage = &xivo_gat::get('datastorage');

		return($datastorage[$type]['extension']);
	}

	function chk_extension_loaded($type,$load=false)
	{
		$type = (string) $type;

		if(xivo_datastorage::chk_extension_exists($type) === false)
			return(false);

		$datastorage = &xivo_gat::get('datastorage');

		if((bool) $load === true)
			blop::load_extension($datastorage[$type]['extension']);

		return(blop::chk_extension_loaded($datastorage[$type]['extension']));
	}

	function chk_extension_exists($type)
	{
		$type = (string) $type;

		if(xivo_datastorage::is_valid($type) === false)
			return(false);

		$datastorage = &xivo_gat::get('datastorage');

		return(xivo_ak('extension',$datastorage[$type]));
	}

	function get_meta($type)
	{
		$type = (string) $type;

		if(xivo_datastorage::is_valid($type) === false)
			return(false);

		$datastorage = &xivo_gat::get('datastorage');

		return(xivo_ak('meta',$datastorage[$type],true));
	}
}

?>
