<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_extenumbers extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'extenumbers';
	var $_filter	= null;
	var $_origin	= false;

	function xivo_service_asterisk_extenumbers(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _prepare($data)
	{
		if(xivo_haslen($data,'exten') === true && $data['exten']{0} === '_')
			$data['exten'] = substr($data['exten'],1);

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		unset($arr['extenhash']);

		if(isset($arr['extenmode']) === true)
			$extenmode = $arr['extenmode'];
		else if(xivo_haslen($arr,'exten') === true)
			$extenmode = 'extension';
		else if(xivo_haslen($arr,'rangebeg') === true
		&& xivo_haslen($arr,'rangeend') === true)
			$extenmode = 'range';
		else
			return(false);

		switch($extenmode)
		{
			case 'extension':
				if(isset($arr['exten']) === false
				|| ($exten = $this->chk_exten($arr['exten'])) === false)
					return(false);

				$arr['extenhash'] = sha1($exten);
				$arr['rangebeg'] = $arr['rangeend'] = null;

				$arr['exten'] = $this->_clean_exten($arr['exten']);
				break;
			case 'range':
				if(isset($arr['rangebeg'],$arr['rangeend']) === false)
					return(false);

				$arr['exten'] = $arr['extenhash'] = '';
				break;
			default:
				return(false);
		}

		$arr['extenmode'] = $extenmode;

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		unset($arr['extenhash']);

		if(isset($arr['extenmode']) === true)
			$extenmode = $arr['extenmode'];
		else if(xivo_haslen($arr,'exten') === true)
			$extenmode = 'extension';
		else if(xivo_haslen($arr,'rangebeg') === true && xivo_haslen($arr,'rangeend') === true)
			$extenmode = 'range';
		else
			$extenmode = '';

		switch($extenmode)
		{
			case 'extension':
				$arr['extenmode'] = 'extension';

				if(isset($arr['exten']) === false)
				{
					unset($arr['rangebeg'],$arr['rangeend'],$arr['exten']);
					break;
				}

				if(($exten = $this->chk_exten($arr['exten'])) === false)
					return(false);

				$arr['extenhash'] = sha1($exten);
				$arr['rangebeg'] = $arr['rangeend'] = null;

				$arr['exten'] = $this->_clean_exten($arr['exten']);
				break;
			case 'range':
				$arr['extenmode'] = 'range';

				if(isset($arr['rangebeg'],$arr['rangeend']) === true)
					break;
			
				unset($arr['rangebeg'],$arr['rangeend'],$arr['exten']);
				break;
			default:
				unset($arr['extenmode'],$arr['rangebeg'],$arr['rangeend'],$arr['exten']);
		}

		return($this->_dso->edit($id,$arr,$disable));
	}

	function exists($arr,$id=null)
	{
		$id = $id !== null ? xivo_uint($id) : null;

		if(is_array($arr) === false || isset($arr['extenmode'],$arr['extenmode']{0}) === false)
			return(null);

		switch($arr['extenmode'])
		{
			case 'extension':
				if(xivo_haslen($arr,'exten') === false
				|| ($exten = $this->chk_exten($arr['exten'])) === false)
					return(null);

				$extenhash = sha1($exten);

				$where = array();
				$where['extenhash'] = $extenhash;
				$where['extenmode'] = $arr['extenmode'];

				$context = xivo_haslen($arr,'context') === true ? $arr['context'] : '';

				if(($whereid = $this->_dso->exists_exten($where,$context)) !== false
				&& ($id === null || ($id !== null && $whereid !== $id) === true) === true)
					return(true);
				break;
			case 'range':
				if(xivo_haslen($arr,'rangebeg') === false
				|| xivo_haslen($arr,'rangeend') === false
				|| $this->chk_range($arr['rangebeg'],$arr['rangeend']) === false)
					return(null);

				$context = xivo_haslen($arr,'context') === true ? $arr['context'] : '';

				if(($rangeid = $this->_dso->exists_range($arr['rangebeg'],$context)) !== false
				&& ($id === null || ($id !== null && $rangeid !== $id) === true) === true)
					return(true);

				if(($rangeid = $this->_dso->exists_range($arr['rangeend'],$context)) !== false
				&& ($id === null || ($id !== null && $rangeid !== $id) === true) === true)
					return(true);

				$where = array();
				$where['extenmode'] = 'extension';
				$where['exten'] = $arr['rangebeg'];

				if($this->_dso->exists_exten($where,$context) !== false)
					return(true);

				$where['exten'] = $arr['rangeend'];

				if($this->_dso->exists_exten($where,$context) !== false)
					return(true);
				break;
			default:
				return(null);
		}

		return(false);
	}

	function chk_exten($str)
	{
		$str = strval($str);

		if(isset($str{0}) === false)
			return(false);

		if(preg_match('/[\[\-\]]/',$str) === 0)
			return($this->_realexten_pattern($str));

		$split = preg_split('/\[(\d+(?:\-\d+)?)\]/',$str,-1,PREG_SPLIT_DELIM_CAPTURE);

		if(($nb = count($split)) === 0)
			return($this->_realexten_pattern($str));

		$r = '';

		for($i = 0;$i < $nb;$i+=2)
		{
			if(preg_match('/[\[\-\]]/',$split[$i]) === 1)
				return(false);
			else
				$r .= $this->_realexten_pattern($split[$i]);

			if(isset($split[$i+1]) === false)
				return($r);

			$nsplit = $split[$i+1];

			$arr = array();

			if(isset($nsplit{1}) === false)
			{
				$r .= $nsplit{0};
				continue;
			}

			if(preg_match('/^(\d*)(\d-\d)(\d*)$/',$nsplit,$match) === 1)
			{
				if($match[2]{0} >= $match[2]{2})
					return(false);

				$arr = array_flip(range($match[2]{0},$match[2]{2}));

				if(($len = strlen($match[1])) !== 0)
				{
					for($j = 0;$j < $len;$j++)
					{
						$num = $match[1]{$j};

						if(isset($arr[$num]) === true)
							return(false);

						$arr[$num] = 1;
					}
				}

				if(($len = strlen($match[3])) !== 0)
				{
					for($j = 0;$j < $len;$j++)
					{
						$num = $match[3]{$j};

						if(isset($arr[$num]) === true)
							return(false);

						$arr[$num] = 1;
					}
				}
			}
			else
			{
				$len = strlen($nsplit);

				for($j = 0;$j < $len;$j++)
				{
					$num = $nsplit{$j};

					if(isset($arr[$num]) === true)
						return(false);

					$arr[$num] = 1;
				}
			}
	
			ksort($arr);

			$r .= '['.implode('',array_keys($arr)).']';
		}

		return($r);
	}

	function chk_range($beg,$end)
	{
		if(ctype_digit($beg) === false || ctype_digit($end) === false || $beg >= $end)
			return(false);

		return(true);
	}

	function _clean_exten($str)
	{
		$str = preg_replace('/\[(\d)\]/','\\1',(string) $str);

		if(isset($str{0}) === true && $str{0} === '_')
		{
			$substr = substr($str,1);

			if(preg_match('/[NXZ\-\[\]\!\.]/',$substr) === 0)
				$str = $substr;

			return($str);
		}
		else if(preg_match('/[NXZ\-\[\]\!\.]/',$str) !== 0)
			$str = '_'.$str;

		return($str);
	}

	function _realexten_pattern($str)
	{
		$str = preg_replace('/\[(\d)\]/','\\1',(string) $str);
		$pat = array('_','X','Z','N');
		$rep = array('','[0123456789]','[123456879]','[23456789]');

		return(str_replace($pat,$rep,$str));
	}

	function _regexten_pattern($str)
	{
		$pat = array('_','*','X','Z','N','.','!');
		$rep = array('','\*','[0-9]','[1-9]','[2-9]','[0-9#\*]+','[0-9#\*]*');

		return(str_replace($pat,$rep,(string) $str));
	}
}

?>
