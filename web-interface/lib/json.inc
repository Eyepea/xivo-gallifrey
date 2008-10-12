<?php

class xivo_json
{
	function load_extension()
	{
		if(blop::load_extension('json') === false)
			trigger_error('Unable to load json',E_USER_ERROR);
	}

	function get_header()
	{
		return('Content-Type: application/json');
	}

	function encode($data)
	{
		xivo_json::load_extension();

		return(json_encode($data));
	}

	function decode($data,$assoc=false)
	{
		if(xivo_haslen($data) === false)
			return(false);

		xivo_json::load_extension();

		return(json_decode($data,(bool) $assoc));
	}
}

?>
