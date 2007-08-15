<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_generaloutcall extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'generaloutcall';
	var $_filter	= null;
	var $_origin	= false;

	function xivo_service_asterisk_generaloutcall(&$sre,&$dso)
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
		$data['commented'] = (bool) $data['commented'];

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

		if(isset($arr['rangebeg'],$arr['rangeend']) === true
		&& $this->chk_range($arr['rangebeg'],$arr['rangeend']) === true)
		{
			if($this->_dso->exists_range($arr['rangebeg']) !== false
			|| $this->_dso->exists_range($arr['rangeend']) !== false)
				return(false);

			$arr['exten'] = $arr['extenhash'] = '';
		}
		else if(isset($arr['exten']) === true
		&& ($exten = $this->chk_exten($arr['exten'])) !== false)
		{
			$arr['rangebeg'] = $arr['rangeend'] = null;
			$arr['extenhash'] = sha1($exten);

			$where = array('extenhash' => $arr['extenhash']);

			if($this->get_id($where) !== false)
				return(false);

			if($arr['exten']{0} === '_')
			{
				$subexten = substr($arr['exten'],1);

				if(ctype_digit($subexten) === true)
					$arr['exten'] = $subexten;
			}
			else if(ctype_digit($arr['exten']) === false)
					$arr['exten'] = '_'.$arr['exten'];
		}
		else
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		unset($arr['extenhash']);

		if(xivo_haslen($arr,'rangebeg') === true
		&& xivo_haslen($arr,'rangeend') === true)
		{
			if($this->chk_range($arr['rangebeg'],$arr['rangeend']) === false)
				return(false);

			if(($rangeid = $this->_dso->exists_range($arr['rangebeg'])) !== false
			&& (int) $rangeid !== $id)
				return(false);

			if(($rangeid = $this->_dso->exists_range($arr['rangeend'])) !== false
			&& (int) $rangeid !== $id)
				return(false);

			$arr['exten'] = $arr['extenhash'] = '';
		}
		else if(xivo_haslen($arr,'exten') === true)
		{
			if(($exten = $this->chk_exten($arr['exten'])) === false)
				return(false);

			$arr['rangebeg'] = $arr['rangeend'] = null;
			$arr['extenhash'] = sha1($exten);

			$where = array('extenhash' => $arr['extenhash']);

			if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
				return(false);

			if($arr['exten']{0} === '_')
			{
				$subexten = substr($arr['exten'],1);

				if(ctype_digit($subexten) === true)
					$arr['exten'] = $subexten;
			}
			else if(ctype_digit($arr['exten']) === false)
					$arr['exten'] = '_'.$arr['exten'];
		}
		else
			unset($arr['rangebeg'],$arr['rangeend'],$arr['exten']);

		return($this->_dso->edit($id,$arr,$disable));
	}

	function chk_exten($str)
	{
		$str = strval($str);

		if(isset($str{0}) === false)
			return(false);

		if(preg_match('/[\[\-\]]/',$str) === 0)
			return($this->_realexten_pattern($str));

		$flag = PREG_SPLIT_DELIM_CAPTURE;

		$split = preg_split('/\[(?:(\d+|\d*(?:\d-\d)\d*))\]/',$str,-1,$flag);

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
		$beg = strval($beg);
		$end = strval($end);

		if($beg === '' || $end === '' || $beg >= $end)
			return(false);

		return(true);
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
