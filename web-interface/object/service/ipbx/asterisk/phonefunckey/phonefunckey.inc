<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_phonefunckey extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'phonefunckey';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;
	var $_type_list		= false;

	function xivo_service_asterisk_phonefunckey(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get_type_list()
	{
		if(is_array($this->_type_list) === true)
			return($this->_type_list);

		if(isset($this->_conf['type']) === false
		|| ($arr = xivo_get_aks($this->_conf['type'])) === false)
			return(false);

		$this->_type_list = array();
		$r = &$this->_type_list;

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_conf['type'][$key];

			if(($arr1 = xivo_get_aks($val)) === false)
			{
				if(is_array($val) === false)
					$r[$key] = $val;
				continue;
			}

			if(isset($r[$key]) === false)
				$r[$key] = array();

			for($j = 0;$j < $arr1['cnt'];$j++)
			{
				$key1 = &$arr1['keys'][$j];
				$val1 = &$val[$key1];

				if(($arr2 = xivo_get_aks($val1)) === false)
				{
					if(is_array($val1) === false)
						$r[$key][$key1.'-'.$val1] = $val1;
					continue;
				}

				for($k = 0;$k < $arr2['cnt'];$k++)
				{
					$key2 = &$arr2['keys'][$k];
					$val2 = &$val1[$key2];

					if(is_bool($val2) === false)
						continue;

					$r[$key][$key1.'-'.$key2] = $val2;
				}
			}
		}

		return($this->_type_list);
	}

	function get($id)
	{
		$this->_origin = false;

		if(is_array($id) === false
		|| isset($id['iduserfeatures'],$id['fknum']) === false
		|| ($id['iduserfeatures'] = xivo_uint($id['iduserfeatures'])) === 0
		|| ($id['fknum'] = xivo_uint($id['fknum'])) === 0
		|| ($r = $this->_dso->get($id)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _origin($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($orig);
		else
			$r = $this->edit(array($orig['iduserfeatures'],$orig['fknum']),$orig);

		return($r);
	}

	function _origin_list($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin_list;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false
		|| ($nb = count($orig)) === 0)
			return(false);

		if($type === 'add')
		{
			for($i = 0;$i < $nb;$i++)
				$this->add($orig[$i]);
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$this->edit(array($orig[$i]['iduserfeatures'],$orig[$i]['fknum']),$orig[$i]);
		}

		return(true);
	}

	function add($arr)
	{
		if(is_array($arr) === false
		|| $this->get_id($arr['iduserfeatures'],$arr['fknum']) !== false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(is_array($id) === false
		|| is_array($arr) === false
		|| isset($id['iduserfeatures'],$id['fknum']) === false
		|| ($id['iduserfeatures'] = xivo_uint($id['iduserfeatures'])) === 0
		|| ($id['fknum'] = xivo_uint($id['fknum'])) === 0)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function delete($id)
	{
		if(is_array($id) === false
		|| isset($id['iduserfeatures'],$id['fknum']) === false
		|| ($id['iduserfeatures'] = xivo_uint($id['iduserfeatures'])) === 0
		|| ($id['fknum'] = xivo_uint($id['fknum'])) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function is_valid($type,$typeval,$id=null,$disable=null)
	{
		switch($type)
		{
			case 'user':
				return($this->chk_dest_user($typeval,$id,$disable,false));
			case 'group':
				return($this->chk_dest_group($typeval,null,$disable,false));
			case 'queue':
				return($this->chk_dest_queue($typeval,null,$disable,false));
			case 'meetme':
				return($this->chk_dest_meetme($typeval,null,$disable,false));
		}

		return(false);
	}

	function get_all($prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(($list = $this->_dso->get_all(null,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);
		else if(($nb = count($list)) === 0)
			return(false);

		$r = array();

		if($assoc === true && $prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$key = $list[$i]['iduserfeatures'].'-'.$list[$i]['fknum'];
				$r[$key] = $this->_prepare($list[$i]);
			}
		}
		else if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$key = $list[$i]['iduserfeatures'].'-'.$list[$i]['fknum'];
				$r[$key] = $list[$i];
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $this->_prepare($list[$i]);
		}

		return($r);
	}

	function get_all_where($arr,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,null,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);
		else if(($nb = count($list)) === 0)
			return(false);

		$r = array();

		if($assoc === true && $prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$key = $list[$i]['iduserfeatures'].'-'.$list[$i]['fknum'];
				$r[$key] = $this->_prepare($list[$i]);
			}
		}
		else if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$key = $list[$i]['iduserfeatures'].'-'.$list[$i]['fknum'];
				$r[$key] = $list[$i];
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $this->_prepare($list[$i]);
		}

		return($r);
	}
}

?>
