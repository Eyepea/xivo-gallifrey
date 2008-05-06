<?php

class xivo_json
{
	var $_recursive = false;
	var $_encoded	= array();

	function xivo_json($recursive=false)
	{
		$this->_recursive = (bool) $recursive;
	}

	function encode($data,$recursive=false)
	{
		$this->_recursive = (bool) $recursive;
		$this->_encoded = array();

		return($this->_encode_data($data));
	}

	function _encode_data(&$data)
	{
		if(is_object($data) === true)
			return($this->_encode_object($data));
		else if(is_array($data) === true)
			return($this->_encode_array($data));

		return($this->_encode_scalar($data));
	}

	function _encode_object(&$data)
	{
		if($this->_recursive === true)
		{
			if($this->_chk_recursive($data) === true)
				trigger_error('Object already encoded: '.get_class($data),E_USER_WARNING);
			else
				$this->_encoded[] = $data;
		}

		$r = '';

		foreach(get_object_vars($data) as $key => $value)
			$r .=	','.$this->_encode_string($key).
				':'.$this->_encode_data($value);

		return('{"__className":"'.get_class($data).'"'.$r.'}');
	}

	function _encode_array(&$data)
	{
		if(empty($data) === true)
			return('[]');

		$r = '';
		$keys = array_keys($data);
		$nb = count($keys);
		
		if(in_array(0,$keys,true) === true && array_keys($data) === range(0,$nb-1))
		{
			for($i = 0;$i < $nb;$i++)
				$r .= ','.$this->_encode_data($data[$i]);

			return('['.substr($r,1).']');
		}

		for($i = 0;$i < $nb;$i++)
			$r .=	','.$this->_encode_string($keys[$i]).
				':'.$this->_encode_data($data[$keys[$i]]);

		return('{'.substr($r,1).'}');
	}

	function _encode_scalar(&$data)
	{
		switch(gettype($data))
		{
			case 'string':
				return($this->_encode_string($data));
			case 'integer':
			case 'double':
				return((string) $data);
			case 'boolean':
				return(($data === true ? 'true' : 'false'));
		}

		return('null');
	}

	function _encode_string(&$data)
	{
		$pat = array('\\', "\n", "\t", "\r", "\b", "\f", '"',"\x08","\x0C");
		$rep = array('\\\\','\\n','\\t','\\r','\\b','\\f','\"','\b','\f');

		return('"'.str_replace($pat,$rep,$data).'"');
	}

	function _chk_recursive(&$data)
	{
		return(in_array($data,$this->_encoded,true));
	}
}

?>
