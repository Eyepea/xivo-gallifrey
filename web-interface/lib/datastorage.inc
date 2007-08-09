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

	function get_param_uri($str)
	{
		xivo::load_class('xivo_uri');
		$uri = new xivo_uri();

		if(($arr = $uri->parse_uri($str)) === false || isset($arr['scheme']) === false)
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

				if(isset($arr['query'][$i][1]) === true)
					$r[$arr['query'][$i][0]] = $arr['query'][$i][1];
				else
					$r[$arr['query'][$i][0]] = '';
			}
		}

		return($r);
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
