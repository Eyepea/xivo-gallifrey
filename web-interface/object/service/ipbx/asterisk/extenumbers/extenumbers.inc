<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_extenumbers extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'extenumbers';
	var $_filter	= false;
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
		if(xivo_haslen($data,'exten') === true)
			$data['exten'] = $this->_sre->display_extension($data['exten']);

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if(xivo_haslen($arr,'exten') === false
		|| ($exten = $this->_sre->chk_extension($arr['exten'])) === false)
			return(false);

		$arr['extenhash'] = sha1($exten);

		$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		unset($arr['extenhash']);

		if(xivo_haslen($arr,'exten') === true)
		{
			if(($exten = $this->_sre->chk_extension($arr['exten'])) === false)
				return(false);

			$arr['extenhash'] = sha1($exten);
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);
		}

		return($this->_dso->edit($id,$arr));
	}

	function exists($arr,$id=null)
	{
		$id = $id !== null ? xivo_uint($id) : null;

		if(is_array($arr) === false
		|| xivo_haslen($arr,'exten') === false
		|| ($exten = $this->_sre->chk_extension($arr['exten'])) === false)
			return(null);

		$extenhash = sha1($exten);

		$where = array();
		$where['extenhash'] = $extenhash;

		$context = xivo_haslen($arr,'context') === true ? $arr['context'] : '';

		if(($whereid = $this->_dso->exists_exten($where,$context)) !== false
		&& ($id === null || $whereid !== $id) === true)
			return(true);

		return(false);
	}

	function exists_type($arr)
	{
		if(is_array($arr) === false
		|| xivo_haslen($arr,'exten') === false
		|| xivo_haslen($arr,'type') === false
		|| ($exten = $this->_sre->chk_extension($arr['exten'])) === false)
			return(null);

		$extenhash = sha1($exten);

		$where = array();
		$where['extenhash'] = $extenhash;

		$typeval = xivo_haslen($arr,'typeval') === true ? $arr['typeval'] : '';
		$context = xivo_haslen($arr,'context') === true ? $arr['context'] : '';

		$keytype = $arr['type'].'-'.$typeval;

		if(($ret = $this->_dso->exists_type($where,$context)) !== false
		&& ($typeval === '' || $ret['type'].'-'.$ret['typeval'] !== $keytype) === true)
			return(true);

		return(false);
	}

	function get_search($exten,$context='',$type='',$typeval='')
	{
		if(($extenchk = $this->_sre->chk_extension($exten)) === false)
			return(false);

		$extenhash = sha1($extenchk);

		if(($list = $this->_dso->get_search($exten,$extenhash,$context,$type,$typeval)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
