<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','abstract.inc'),true);

class xivo_service_asterisk_generaloutcall extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'generaloutcall';
	var $_filter	= null;
	var $_origin	= null;

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

	function get($id,$disable=null)
	{
		if(is_array($id) === true && ($id = $this->get_where($id)) === false)
			return(false);

		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function add_origin()
	{
		return($this->_origin('add'));
	}

	function edit_origin()
	{
		return($this->_origin('edit'));
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,$this->_origin['id']);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if(isset($arr['number']) === true)
		{
			$where = array('number' => $arr['number']);

			if(($whereid = $this->get_where($where)) !== false)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['number']) === true)
		{
			$where = array('number' => $arr['number']);

			if(($whereid = $this->get_where($where)) !== false && (int) $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr));
	}

	function edit_where($arr,$values)
	{
		$arr = (array) $arr;

		if(is_array($values) === false
		|| ($where = $this->get_elements_where($arr)) === false
		|| ($values = $this->chk_set_values($values)) === false)
			return(false);

		return($this->_dso->edit_where($where,$values));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function get_where($arr)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false)
			return(false);

		return($this->_dso->get_where($where));
	}

	function get_all_where($arr=array(),$disable=null,$prepare=true)
	{
		$arr = (array) $arr;
		$prepare = (bool) $prepare;

		if(($where = $this->get_elements_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $prepare === true ? $this->_prepare($list[$i]) : $list[$i];

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function _realexten_pattern($str)
	{
		$str = preg_replace('/(\d)/','[\1]',(string) $str);
		$pat = array('X','Z','N');
		$rep = array('[0123456789]','[123456879]','[23456789]');

		return(str_replace($pat,$rep,$str));
	}

	function _chk_exten($str)
	{
		$str = (string) $str;

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

	function _regexten_pattern($str)
	{
		$pat = array('X','Z','N','.','!');
		$rep = array('[0-9]','[1-9]','[2-9]','+','*');

		return(str_replace($pat,$rep,(string) $str));
	}
}

?>
