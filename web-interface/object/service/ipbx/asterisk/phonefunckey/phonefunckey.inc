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

	function _prepare($data)
	{
		$data['type'] = '';
		$data['typeval'] = '';
		$data['user'] = '';
		$data['group'] = '';
		$data['queue'] = '';
		$data['meetme'] = '';
		$data['extension'] = '';
		$data['bosssecretary'] = '';
		$data['custom'] = '';
		$data['supervision'] = (bool) $data['supervision'];

		switch($data['typeextenumbers'])
		{
			case 'extenfeatures':
				if($data['typevalextenumbers'] === 'bsfilter')
				{
					$data['type'] = 'bosssecretary';
					$data['bosssecretary'] = $data['typeval'] = $data['typevalextenumbersright'];
					return($data);
				}
			case 'featuremap':
			case 'generalfeatures':
				$data['type'] = 'extension';
				$data['typeval'] = $data['typevalextenumbers'];
				$data['extension'] = $data['typeextenumbers'].'-'.$data['typevalextenumbers'];
				return($data);
		}

		switch($data['typeextenumbersright'])
		{
			case 'user':
			case 'group':
			case 'queue':
			case 'meetme':
				$data['type'] = $data['typeextenumbersright'];
				$data[$data['typeextenumbersright']] = $data['typeval'] = $data['typevalextenumbersright'];
				return($data);
		}

		if(xivo_haslen($data['exten']) === true)
		{
			$data['type'] = 'custom';
			$data['custom'] = $data['typeval'] = $data['exten'];
		}

		return($data);
	}

	function get_type_list()
	{
		if(is_array($this->_type_list) === true)
			return($this->_type_list);

		if(isset($this->_conf['type']) === false
		|| is_array($this->_conf['type']) === false
		|| empty($this->_conf['type']) === true)
			return(false);

		$this->_type_list = array();
		$r = &$this->_type_list;

		foreach($this->_conf['type'] as $key => $val)
		{
			if(is_array($val) === false
			|| empty($val) === true)
			{
				if(is_array($val) === false)
					$r[$key] = $val;
				continue;
			}
			else if(isset($r[$key]) === false)
				$r[$key] = array();

			foreach($val as $key1 => $val1)
			{
				if(is_array($val1) === false
				|| empty($val1) === true)
				{
					if(is_array($val1) === false)
						$r[$key][$key1.'-'.$val1] = $val1;
					continue;
				}

				foreach($val1 as $key2 => $val2)
				{
					if(is_bool($val2) === true)
						$r[$key][$key1.'-'.$key2] = $val2;
				}
			}
		}

		return($this->_type_list);
	}

	function mk_values_array($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['fknum'],$arr['type'],$arr['typeval'],$arr['iduserfeatures']) === false
		|| $this->chk_element_value('type',$arr['type']) === false
		|| xivo_haslen($arr['typeval']) === false)
			return(false);

		$r = array();
		$r['iduserfeatures'] = $arr['iduserfeatures'];
		$r['fknum'] = $arr['fknum'];
		$r['exten'] = '';
		$r['typeextenumbers'] = '';
		$r['typevalextenumbers'] = '';
		$r['typeextenumbersright'] = '';
		$r['typevalextenumbersright'] = '';
		$r['supervision'] = 0;

		switch($arr['type'])
		{
			case 'extension':
				$val = explode('-',$arr['typeval'],2);

				if(xivo_haslen($val,0) === false
				|| xivo_haslen($val,1) === false)
					return(false);

				$r['typeextenumbers'] = $val[0];
				$r['typevalextenumbers'] = $val[1];
				break;
			case 'bosssecretary':
				$r['typeextenumbers'] = 'extenfeatures';
				$r['typevalextenumbers'] = 'bsfilter';
				$r['typeextenumbersright'] = 'user';
				$r['typevalextenumbersright'] = $arr['typeval'];

				if(isset($arr['supervision']) === true)
					$r['supervision'] = $arr['supervision'];
				break;
			case 'custom':
				$r['exten'] = $arr['typeval'];
				break;
			case 'user':
				if(isset($arr['supervision']) === true)
					$r['supervision'] = $arr['supervision'];
			default:
				$r['typeextenumbersright'] = $arr['type'];
				$r['typevalextenumbersright'] = $arr['typeval'];
		}

		return($r);
	}

	function is_valid($type='',$typeval='',$typeright='',$typevalright='',$id=null,$disable=null,$context=null)
	{
		switch($type)
		{
			case 'extenfeatures':
				if($typeval === 'bsfilter')
					return(true);
			case 'featuremap':
			case 'generalfeatures':
				if(($extentype = $this->_chk_exists_cfg('type','extension',true)) === false
				|| xivo_issa($type,$extentype) === false
				|| isset($extentype[$type][$typeval]) === false)
					return(false);
				return(true);
		}

		switch($typeright)
		{
			case 'user':
				return($this->chk_dest_user($typevalright,$id,$disable,true,true,$context));
			case 'group':
				return($this->chk_dest_group($typevalright,null,$disable,true,true,$context));
			case 'queue':
				return($this->chk_dest_queue($typevalright,null,$disable,true,true,$context));
			case 'meetme':
				return($this->chk_dest_meetme($typevalright,null,$disable,true,true,$context));
		}

		return(false);
	}
}

?>
